#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "map_h.hpp"
#include "turret_menu.hpp"
#include "pathfinder.hpp"
#include "game_state.hpp"
#include "turret_h.hpp"
#include "map_menu.hpp"
#include "enemies.hpp"
#include "sprite.hpp"
#include <algorithm>
#include "enums.hpp"
#include "utils.hpp"
#include <fstream>
#include <vector>
#include <string>

namespace game {
    MapData::MapData(
        const std::vector<std::vector<int>>& initMap,
        const std::vector<SDL_Texture*>& initTextures
    ) :
        textures{initTextures},
        map{initMap} {}

    void Map::updateMapSize() {
        if (
            this->destRect->w != -1
        ) {
            this->tileSize.x
                = this->destRect->w
                / this->mapRef->size();
        }

        if (
            this->destRect->h != -1
        ) {
            this->tileSize.y
                = (
                    this->destRect->h
                    - (this->guiHeight * 3)
                ) / this->mapRef->back().size();
        }
    }

    void Map::updateMapPosition() {
        if (this->gameState != nullptr) this->gameState->setPosition(this->getPosition());
    }

    void Map::updateMapMenu() {
        if (this->mapMenu != nullptr) this->mapMenu->setDestRect(this->getMapDestRect());
    }

    bool Map::efficientPathfindToMultipleTargets(const IPoint& origin, std::vector<std::vector<IPoint>>& paths) {
        for (const IPoint& base : this->bases) {
            typename TileGraph::Nodes::iterator
                o = this->graph->find(origin),
                b = this->graph->find(base);
            if (o == this->graph->end() || b == this->graph->end()) return false;
            std::vector<TileGraph::Node*> foundPath = this->graph->aStar(
                *o, *b,
                [&](TileGraph::Node* a, TileGraph::Node* b) -> double {
                    return distance(a->getValue(), b->getValue());
                }
            );
            if (foundPath.size() == 0) return false;
            std::vector<IPoint> path;
            for (TileGraph::Node* node : foundPath) path.push_back(node->getValue());
            paths.push_back(path);
        }
        return true;
    }

    Map::Map(
        SDL_Renderer* initRenderer,
        TTF_Font* initFont,
        SDL_Rect* initDestRect,
        const IPoint& initTileSize,
        const std::string& initTitle,
        int initGuiHeight
    ) :
        Renderable{
            initRenderer,
            initDestRect
        },
        font{initFont},
        graph{new TileGraph()},
        tileSize{initTileSize},
        title{initTitle},
        guiHeight{initGuiHeight} {

        std::fstream
            textureAssociation("assets/config/map_texture_association.txt", std::ios_base::in),
            mapList("assets/config/maps.txt", std::ios_base::in);
        std::string buffer;

        while (!textureAssociation.eof()) {
            std::getline(textureAssociation, buffer);
            SDL_Surface* surface = IMG_Load(((std::string)"assets/images/tiles/" + buffer).c_str());
            this->textures.push_back(
                SDL_CreateTextureFromSurface(
                    this->renderer,
                    surface
                )
            );
            SDL_FreeSurface(surface);
        }

        textureAssociation.close();

        while (!mapList.eof()) {
            std::getline(mapList, buffer);
            this->mapNames.push_back(buffer);
        }

        mapList.close();

        this->deathBackground = this->textures[TileType::Wall];

        this->mapMenu = new MapMenu(
            this->renderer,
            this->getMapDestRect(),
            this,
            this->mapNames
        );

        this->turretHandler = new TurretHandler(
            this->renderer,
            this->getDestRect(),
            this
        );

        this->turretMenu = new TurretMenu(
            this->renderer,
            this->getTurretMenuDestRect(),
            this->turretHandler->getTurretTypes(),
            this->tileSize
        );

        this->updateMapMenu();
    }

    Map::~Map() {
        for (SDL_Texture* texture : this->textures) SDL_DestroyTexture(texture);
        for (Path* path : this->paths) delete path;
        for (const std::pair<
            std::string,
            std::pair<
                std::pair<GameState*, TileGraph*>,
                std::vector<std::vector<int>>
            >
        >& mapData : this->maps) {
            delete mapData.second.first.first;
            delete mapData.second.first.second;
        }
        delete this->enemyHandler;
        delete this->mapMenu;
    }

    void Map::render() {
        std::vector<std::vector<int>>& map = *this->mapRef;
        if (!this->dead) {
            for (int i = 0; i < map.size(); i++) for (int j = 0; j < map[i].size(); j++) {
                int type = map[i][j];
                SDL_Rect* tileDest = this->getTileDest({i, j});
                SDL_RenderCopy(
                    this->renderer,
                    this->textures.at(
                        type == TileType::TurretType
                        ? TileType::Empty
                        : type
                    ),
                    nullptr,
                    tileDest
                );
                delete tileDest;
            }
            this->enemyHandler->render();
        } else {
            this->deathText->render();
            SDL_RenderCopy(
                this->renderer,
                this->deathBackground,
                nullptr,
                this->destRect
            );
        }
        this->gameState->render();
        this->mapMenu->render();
        this->turretHandler->render();
        this->turretMenu->render();
    }

    void Map::tick(double scalar) {
        this->enemyHandler->tick(scalar);
        this->turretHandler->tick(scalar);
    }

    void Map::handleEvent(SDL_Event* event) {
        this->turretHandler->handleEvent(event);
        this->mapMenu->handleEvent(event);
        this->turretMenu->handleEvent(event);
    }

    GameState* Map::loadMap(const std::string& mapFileName) {
        this->dead = false;
        this->map = mapFileName;
        if (this->maps.find(this->map) != this->maps.end()) {
            this->maps[this->map].first.first->reset();
            this->gameState = this->maps[this->map].first.first;
            this->graph = this->maps[this->map].first.second;
            this->mapRef = &(this->maps[this->map].second);
            for (Path* path : this->paths) delete path;
            this->paths.clear();
            this->spawns.clear();
            this->bases.clear();

            for (int i = 0; i < this->mapRef->size(); i++) for (int j = 0; j < this->mapRef->back().size(); j++) {
                int type = this->mapRef->at(i).at(j);
                if (type == TileType::Spawn) this->spawns.push_back({i, j});
                else if (type == TileType::Base) this->bases.push_back({i, j});
            }

            if (this->enemyHandler != nullptr) delete this->enemyHandler;
            this->enemyHandler = new EnemyHandler(
                this->renderer,
                this->getDestRect(),
                this,
                this->gameState
            );

            std::vector<std::vector<IPoint>> paths;
            for (IPoint& spawn : this->spawns) this->efficientPathfindToMultipleTargets(spawn, paths);
            for (std::vector<IPoint>& path : paths) this->paths.push_back(new Path(this, path));
            return this->maps[this->map].first.first;
        }

        std::fstream
            mapFile("assets/maps/" + mapFileName, std::ios_base::in);
        std::string buffer;

        std::getline(mapFile, buffer);
        int health = std::stoi(buffer);
        std::getline(mapFile, buffer);
        int cash = std::stoi(buffer);
        this->maps.insert({
            mapFileName,
            {
                {nullptr, new TileGraph()},
                {}
            }
        });

        this->mapRef = &this->maps[this->map].second;
        std::vector<std::vector<int>>& newMap = *this->mapRef;
        this->graph = this->maps[this->map].first.second;

        while (!mapFile.eof()) {
            std::getline(mapFile, buffer);
            newMap.push_back({});
            newMap.back().push_back(0);
            for (char c : buffer) {
                if (c == ',') newMap.back().push_back(0);
                else if (c >= '0' && c <= '9') (newMap.back().back() *= 10) += (int)(c - '0');
                else {
                    SDL_Log("`loadMap` failed parsing a map file. Check that syntax is correct in the file.");
                    throw 1;
                }
            }
        }

        mapFile.close();

        const int height = newMap.back().size();
        for (std::vector<int> column : newMap) if (column.size() != height) {
            SDL_Log("`loadMap`: map must be rectangular.");
            throw 1;
        }

        this->spawns.clear();
        this->bases.clear();

        for (int i = 0; i < newMap.size(); i++) {
            for (int j = 0; j < newMap[i].size(); j++) {
                int type = newMap[i][j];
                IPoint index{i, j};

                if (type == TileType::Spawn) this->spawns.push_back(index);
                else if (type == TileType::Base) this->bases.push_back(index);

                if (
                    type != TileType::Empty
                    && type != TileType::Spawn
                    && type != TileType::Base
                ) continue;
                std::pair<TileGraph::Nodes::iterator, bool> r = this->graph->insert(index);
                TileGraph::Node* node = *r.first;

                if (
                    i > 0
                    && (
                        newMap[i - 1][j] == TileType::Empty
                        || newMap[i - 1][j] == TileType::Spawn
                        || newMap[i - 1][j] == TileType::Base
                    )
                ) {
                    (*this->graph->find({i - 1, j}))->link(node);
                }

                if (
                    j > 0
                    && (
                        newMap[i][j - 1] == TileType::Empty
                        || newMap[i][j - 1] == TileType::Spawn
                        || newMap[i][j - 1] == TileType::Base
                    )
                ) {
                    (*this->graph->find({i, j - 1}))->link(node);
                }

                if (
                    i > 0 && j > 0
                    && (
                        newMap[i - 1][j - 1] == TileType::Empty
                        || newMap[i - 1][j - 1] == TileType::Spawn
                        || newMap[i - 1][j - 1] == TileType::Base
                    ) && (
                        newMap[i][j - 1] == TileType::Empty
                        || newMap[i][j - 1] == TileType::Spawn
                        || newMap[i][j - 1] == TileType::Base
                    ) && (
                        newMap[i - 1][j] == TileType::Empty
                        || newMap[i - 1][j] == TileType::Spawn
                        || newMap[i - 1][j] == TileType::Base
                    )
                ) {
                    (*this->graph->find({i - 1, j - 1}))->link(node, sqrtOf2);
                }

                if (
                    i > 0 && j < newMap.back().size() - 1
                    && (
                        newMap[i - 1][j + 1] == TileType::Empty
                        || newMap[i - 1][j + 1] == TileType::Spawn
                        || newMap[i - 1][j + 1] == TileType::Base
                    ) && (
                        newMap[i][j + 1] == TileType::Empty
                        || newMap[i][j + 1] == TileType::Spawn
                        || newMap[i][j + 1] == TileType::Base
                    ) && (
                        newMap[i - 1][j] == TileType::Empty
                        || newMap[i - 1][j] == TileType::Spawn
                        || newMap[i - 1][j] == TileType::Base
                    )
                ) {
                    (*this->graph->find({i - 1, j + 1}))->link(node, sqrtOf2);
                }
            }
        }
        this->updateMapSize();
        this->updateMapPosition();
        this->updateMapMenu();

        for (Path* path : this->paths) delete path;
        this->paths.clear();
        std::vector<std::vector<IPoint>> paths;
        for (IPoint& spawn : this->spawns) this->efficientPathfindToMultipleTargets(spawn, paths);
        for (std::vector<IPoint>& path : paths) this->paths.push_back(new Path(this, path));

        this->gameState = this->maps[this->map].first.first = new GameState(
            this->renderer,
            this->font,
            createRect(
                this->getPosition(),
                IPoint{
                    this->getSize().x,
                    this->guiHeight
                }
            ),
            this->title,
            cash,
            health
        );
        if (this->enemyHandler != nullptr) delete this->enemyHandler;
        this->enemyHandler = new EnemyHandler(
            this->renderer,
            this->getDestRect(),
            this,
            this->gameState
        );
        return this->gameState;
    }

    void Map::start(Option option) {
        this->mapMenu->setDisplayed(false);
        this->enemyHandler->start(option);
        this->turretHandler->start(option);
    }

    void Map::displayDeathScreen() {
        this->dead = true;
    }

    bool Map::placeTurret(const IPoint& index) {
        std::vector<std::vector<int>>& map = *this->mapRef;
        if (
            index.x < 0
            || index.x >= map.size()
            || index.y < 0
            || index.y >= map.back().size()
            || map[index.x][index.y] != TileType::Empty
        ) return false;
        map[index.x][index.y] = TileType::TurretType;
        TileGraph::Node* node = *this->graph->find(index);
        std::vector<std::vector<IPoint>> paths;

        this->graph->erase(node);

        TileGraph::Nodes::iterator
            nodeN = this->graph->find({index.x, index.y - 1}),
            nodeNE = this->graph->find({index.x + 1, index.y - 1}),
            nodeE = this->graph->find({index.x + 1, index.y}),
            nodeSE = this->graph->find({index.x + 1, index.y + 1}),
            nodeS = this->graph->find({index.x, index.y + 1}),
            nodeSW = this->graph->find({index.x - 1, index.y + 1}),
            nodeW = this->graph->find({index.x - 1, index.y}),
            nodeNW = this->graph->find({index.x - 1, index.y - 1});
        if (nodeN != this->graph->end()) {
            if (nodeE != this->graph->end()) (*nodeN)->unlink(*nodeE);
            if (nodeW != this->graph->end()) (*nodeN)->unlink(*nodeW);
        }
        if (nodeS != this->graph->end()) {
            if (nodeE != this->graph->end()) (*nodeS)->unlink(*nodeE);
            if (nodeW != this->graph->end()) (*nodeS)->unlink(*nodeW);
        }

        std::function<void()> relink = [&]() -> void {
            this->graph->insert(node);
            TileGraph::Nodes::iterator end = this->graph->end();

            if (nodeN != end) {
                node->link(*nodeN);
                if (nodeE != end && nodeNE != end) (*nodeN)->link(*nodeE, sqrtOf2);
                if (nodeW != end && nodeNW != end) (*nodeN)->link(*nodeW, sqrtOf2);
            }
            if (nodeNE != end && nodeN != end && nodeE != end) node->link(*nodeNE, sqrtOf2);
            if (nodeE != end) node->link(*nodeE);
            if (nodeSE != end && nodeS != end && nodeE != end) node->link(*nodeSE, sqrtOf2);
            if (nodeS != end) {
                node->link(*nodeS);
                if (nodeE != end && nodeSE != end) (*nodeS)->link(*nodeE, sqrtOf2);
                if (nodeW != end && nodeSW != end) (*nodeS)->link(*nodeW, sqrtOf2);
            }
            if (nodeSW != end && nodeS != end && nodeW != end) node->link(*nodeSW, sqrtOf2);
            if (nodeW != end) node->link(*nodeW);
            if (nodeNW != end && nodeN != end && nodeW != end) node->link(*nodeNW, sqrtOf2);

            map[index.x][index.y] = TileType::Empty;
        };

        for (IPoint& spawn : this->spawns) {
            bool res = this->efficientPathfindToMultipleTargets(spawn, paths);
            if (!res) {
                relink();
                return false;
            }
        }

        for (Enemy* enemy : *this->enemyHandler) {
            IPoint origin = this->getTileIndex(enemy->getCenter());
            if (origin == index) {
                relink();
                return false;
            }
            for (std::vector<IPoint>& path : paths) if (std::find(path.begin(), path.end(), origin) != path.end()) continue;
            bool res = this->efficientPathfindToMultipleTargets(origin, paths);
            if (!res) {
                relink();
                return false;
            }
        }

        for (Path* path : this->paths) delete path;
        this->paths.clear();

        for (std::vector<IPoint>& path : paths) this->paths.push_back(new Path(this, path));

        for (Enemy* enemy : *this->enemyHandler) {
            IPoint origin = this->getTileIndex(enemy->getCenter());
            std::vector<Path*> validPaths;
            for (Path* path : this->paths) if (path->isIndexInPath(origin)) validPaths.push_back(path);
            enemy->setPath(validPaths[std::rand() % validPaths.size()]);
        }

        return true;
    }

    bool Map::sellTurret(const IPoint& index) {
        std::vector<std::vector<int>>& map = this->maps[this->map].second;
        if (
            index.x < 0
            || index.x >= map.size()
            || index.y < 0
            || index.y >= map.back().size()
            || map[index.x][index.y] != TileType::TurretType
        ) return false;

        typename TileGraph::Nodes::iterator
            nodeN = this->graph->find({index.x, index.y - 1}),
            nodeNE = this->graph->find({index.x + 1, index.y - 1}),
            nodeE = this->graph->find({index.x + 1, index.y}),
            nodeSE = this->graph->find({index.x + 1, index.y + 1}),
            nodeS = this->graph->find({index.x, index.y + 1}),
            nodeSW = this->graph->find({index.x - 1, index.y + 1}),
            nodeW = this->graph->find({index.x - 1, index.y}),
            nodeNW = this->graph->find({index.x - 1, index.y - 1});
        TileGraph::Node* node = *(this->graph->insert(index).first);

        TileGraph::Nodes::iterator end = this->graph->end();
        if (nodeN != end) {
            node->link(*nodeN);
            if (nodeE != end && nodeNE != end) (*nodeN)->link(*nodeE, sqrtOf2);
            if (nodeW != end && nodeNW != end) (*nodeN)->link(*nodeW, sqrtOf2);
        }
        if (nodeNE != end && nodeN != end && nodeE != end) node->link(*nodeNE, sqrtOf2);
        if (nodeE != end) node->link(*nodeE);
        if (nodeSE != end && nodeS != end && nodeE != end) node->link(*nodeSE, sqrtOf2);
        if (nodeS != end) {
            node->link(*nodeS);
            if (nodeE != end && nodeSE != end) (*nodeS)->link(*nodeE, sqrtOf2);
            if (nodeW != end && nodeSW != end) (*nodeS)->link(*nodeW, sqrtOf2);
        }
        if (nodeSW != end && nodeS != end && nodeW != end) node->link(*nodeSW, sqrtOf2);
        if (nodeW != end) node->link(*nodeW);
        if (nodeNW != end && nodeN != end && nodeW != end) node->link(*nodeNW, sqrtOf2);

        map[index.x][index.y] = TileType::Empty;

        for (Path* path : this->paths) delete path;
        this->paths.clear();

        std::vector<std::vector<IPoint>> paths;
        for (IPoint& spawn : this->spawns) this->efficientPathfindToMultipleTargets(spawn, paths);

        for (Enemy* enemy : *this->enemyHandler) {
            IPoint origin = this->getTileIndex(enemy->getCenter());
            for (std::vector<IPoint>& path : paths) if (std::find(path.begin(), path.end(), origin) != path.end()) continue;
            bool res = this->efficientPathfindToMultipleTargets(origin, paths);
        }

        for (std::vector<IPoint>& path : paths) this->paths.push_back(new Path(this, path));

        for (Enemy* enemy : *this->enemyHandler) {
            IPoint origin = this->getTileIndex(enemy->getCenter());
            std::vector<Path*> validPaths;
            for (Path* path : this->paths) if (path->isIndexInPath(origin)) validPaths.push_back(path);
            enemy->setPath(validPaths[std::rand() % validPaths.size()]);
        }

        return true;
    }

    void Map::setDestRect(SDL_Rect* newDestRect) {
        delete this->destRect;
        this->destRect = newDestRect;
        this->updateMapPosition();
        this->updateMapMenu();
    }

    void Map::setPosition(const IPoint& position) {
        this->Renderable::setPosition(position);
        this->updateMapPosition();
        this->updateMapMenu();
    }

    SDL_Rect* Map::getMapDestRect() const {
        SDL_Rect* rect = new SDL_Rect();
        rect->x = this->destRect->x;
        rect->y = this->destRect->y + this->guiHeight;
        if (this->maps.empty()) rect->w = this->tileSize.x;
        else rect->w = this->tileSize.x * this->mapRef->size();
        if (this->maps.empty() || this->mapRef->empty()) rect->h = this->tileSize.y;
        else rect->h = this->tileSize.y * this->mapRef->back().size();
        return rect;
    }

    SDL_Rect* Map::getTurretMenuDestRect() const {
        SDL_Rect* rect = new SDL_Rect();
        rect->x = this->destRect->x;
        rect->y = this->destRect->y + (this->tileSize.y * this->mapRef->back().size()) + this->guiHeight;
        if (this->maps.empty()) rect->w = this->tileSize.x;
        else rect->w = this->tileSize.x * this->mapRef->size();
        rect->h = this->guiHeight << 1;
        return rect;
    }

    SDL_Rect* Map::getDestRect() const {
        SDL_Rect* rect = new SDL_Rect();
        rect->x = this->destRect->x;
        rect->y = this->destRect->y;
        if (this->maps.empty()) rect->w = this->tileSize.x;
        else rect->w = this->tileSize.x * this->mapRef->size();
        if (this->maps.empty() || this->mapRef->empty()) rect->h = this->guiHeight * 3;
        else rect->h = (this->tileSize.y * this->mapRef->back().size()) + (this->guiHeight * 3);
        return rect;
    }

    IPoint Map::getSize() const {
        if (map.empty()) return {0, this->guiHeight * 3};
        else return {
            this->tileSize.x * (int)this->mapRef->size(),
            (this->tileSize.y * (int)this->mapRef->back().size()) + (this->guiHeight * 3)
        };
    }

    IPoint Map::getTileIndex(IPoint position) const {
        SDL_Rect* rect = this->getDestRect();
        if (
            position.x < rect->x
            || position.y < rect->y + this->guiHeight
            || position.x > rect->x + (this->mapRef->size() * this->tileSize.x)
            || position.y > rect->y + (this->mapRef->front().size() * this->tileSize.y) + this->guiHeight
        ) {
            delete rect;
            return {-1, -1};
        }

        delete rect;
        return {
            (int)std::floor((double)(position.x - this->destRect->x) / (double)this->tileSize.x),
            (int)std::floor((double)(position.y - (this->destRect->y + this->guiHeight)) / (double)this->tileSize.y)
        };
    }

    IPoint Map::getTileIndexCenter(SDL_Rect* rect) const {
        return this->getTileIndex(getCenter(rect));
    }

    SDL_Rect* Map::getTileDest(const IPoint& index) const {
        SDL_Rect* rect = new SDL_Rect();
        rect->x = this->destRect->x + (this->tileSize.x * index.x);
        rect->y = this->destRect->y + (this->tileSize.y * index.y) + this->guiHeight;
        rect->w = this->tileSize.x;
        rect->h = this->tileSize.y;
        return rect;
    }

    IPoint Map::getTileCenter(const IPoint& index) const {
        return {
            this->destRect->x + (this->tileSize.x * index.x) + (this->tileSize.x >> 1),
            this->destRect->y + (this->tileSize.y * index.y) + (this->tileSize.y >> 1) + this->guiHeight
        };
    }

    int Map::getTileType(const IPoint& index) const {
        if (index.x < 0 || index.x >= this->mapRef->size() || index.y < 0 || index.y >= this->mapRef->back().size()) return TileType::Wall;
        return (*this->mapRef)[index.x][index.y];
    }

    const std::vector<IPoint>& Map::getAllSpawns() const {
        return this->spawns;
    }

    const std::vector<IPoint>& Map::getAllBases() const {
        return this->bases;
    }

    IPoint Map::getTileSize() const {
        return this->tileSize;
    }

    const std::vector<Path*>& Map::getPaths() const {
        return this->paths;
    }

    GameState* Map::getGameState() {
        return this->gameState;
    }

    EnemyHandler* Map::getEnemyHandler() {
        return this->enemyHandler;
    }

    TurretMenu* Map::getTurretMenu() {
        return this->turretMenu;
    }
};
