#pragma once
#include <SDL2/SDL.h>
#include "map_h.hpp"
#include "pathfinder.hpp"
#include "game_state.hpp"
#include "enemies.hpp"
#include "sprite.hpp"
#include <algorithm>
#include "enums.hpp"
#include "utils.hpp"
#include <fstream>
#include <vector>
#include <string>

namespace game {
    void Map::updateMapSize() {
        if (
            this->destRect->w != -1
        ) {
            this->tileSize.x
                = this->destRect->w
                / this->map.size();
        }

        if (
            this->destRect->h != -1
        ) {
            this->tileSize.y
                = (
                    this->destRect->h
                    - this->headerHeight
                ) / this->map.back().size();
        }
    }

    void Map::updateTiles() {
        for (int i = 0; i < this->mapSprites.size(); i++) {
            for (int j = 0; j < this->mapSprites[i].size(); j++) {
                this->mapSprites[i][j]->setDestRect(this->getTileDest({i, j}));
            }
        }
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
        SDL_Rect* initDestRect,
        const IPoint& initTileSize,
        int initHeaderHeight
    ) :
        Renderable{
            initRenderer,
            initDestRect
        },
        tileSize{initTileSize},
        graph{new TileGraph()},
        headerHeight{initHeaderHeight} {
        std::fstream
            textureAssociation("assets/config/map_texture_association.txt", std::ios_base::in);
        std::string buffer;

        while (!textureAssociation.eof()) {
            std::getline(textureAssociation, buffer);
            SDL_Surface* surface = SDL_LoadBMP(((std::string)"assets/images/" + buffer).c_str());
            this->textures.push_back(
                SDL_CreateTextureFromSurface(
                    this->renderer,
                    surface
                )
            );
            SDL_FreeSurface(surface);
        }

        textureAssociation.close();
    }

    Map::~Map() {
        for (std::vector<StaticSprite*>& col : this->mapSprites) for (StaticSprite* sprite : col) delete sprite;
        for (SDL_Texture* texture : this->textures) SDL_DestroyTexture(texture);
        for (Path* path : this->paths) delete path;
        delete this->graph;
        delete this->enemyHandler;
    }

    void Map::render() {
        for (int i = 0; i < this->map.size(); i++) for (int j = 0; j < this->map[i].size(); j++) {
            int type = this->map[i][j];
            SDL_RenderCopy(
                this->renderer,
                this->textures.at(
                    type == TileType::TurretType
                    ? TileType::Wall
                    : type
                ),
                nullptr,
                this->mapSprites[i][j]->getDestRect()
            );
        }
        this->enemyHandler->render();
    }

    void Map::tick(double scalar) {
        this->enemyHandler->tick(scalar);
    }

    GameState* Map::loadMap(const std::string& mapFileName) {
        std::fstream
            mapFile("assets/maps/" + mapFileName, std::ios_base::in);
        std::string buffer;

        std::getline(mapFile, buffer);
        int health = std::stoi(buffer);
        std::getline(mapFile, buffer);
        int cash = std::stoi(buffer);

        while (!mapFile.eof()) {
            std::getline(mapFile, buffer);
            map.push_back({});
            map.back().push_back(0);
            for (char c : buffer) {
                if (c == ',') map.back().push_back(0);
                else if (c >= '0' && c <= '9') (map.back().back() *= 10) += (int)(c - '0');
                else {
                    SDL_Log("`loadMap` failed parsing a map file. Check that syntax is correct in the file.");
                    throw 1;
                }
            }
        }

        const int height = this->map.back().size();
        for (std::vector<int> column : this->map) if (column.size() != height) {
            SDL_Log("`loadMap`: map must be rectangular.");
            throw 1;
        }

        for (int i = 0; i < this->map.size(); i++) {
            this->mapSprites.push_back({});
            for (int j = 0; j < this->map[i].size(); j++) {
                int type = this->map[i][j];
                IPoint index{i, j};
                this->mapSprites.back().push_back(new StaticSprite(
                    this->renderer,
                    this->textures.at(type),
                    this->getTileDest(index)
                ));
                this->mapSprites[i][j]->setDestRect(this->getTileDest({i, j}));

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
                        this->map[i - 1][j] == TileType::Empty
                        || this->map[i - 1][j] == TileType::Spawn
                        || this->map[i - 1][j] == TileType::Base
                    )
                ) {
                    (*this->graph->find({i - 1, j}))->link(node);
                }

                if (
                    j > 0
                    && (
                        this->map[i][j - 1] == TileType::Empty
                        || this->map[i][j - 1] == TileType::Spawn
                        || this->map[i][j - 1] == TileType::Base
                    )
                ) {
                    (*this->graph->find({i, j - 1}))->link(node);
                }

                if (
                    i > 0 && j > 0
                    && (
                        this->map[i - 1][j - 1] == TileType::Empty
                        || this->map[i - 1][j - 1] == TileType::Spawn
                        || this->map[i - 1][j - 1] == TileType::Base
                    ) && (
                        this->map[i][j - 1] == TileType::Empty
                        || this->map[i][j - 1] == TileType::Spawn
                        || this->map[i][j - 1] == TileType::Base
                    ) && (
                        this->map[i - 1][j] == TileType::Empty
                        || this->map[i - 1][j] == TileType::Spawn
                        || this->map[i - 1][j] == TileType::Base
                    )
                ) {
                    (*this->graph->find({i - 1, j - 1}))->link(node, sqrtOf2);
                }

                if (
                    i > 0 && j < this->map.back().size() - 1
                    && (
                        this->map[i - 1][j + 1] == TileType::Empty
                        || this->map[i - 1][j + 1] == TileType::Spawn
                        || this->map[i - 1][j + 1] == TileType::Base
                    ) && (
                        this->map[i][j + 1] == TileType::Empty
                        || this->map[i][j + 1] == TileType::Spawn
                        || this->map[i][j + 1] == TileType::Base
                    ) && (
                        this->map[i - 1][j] == TileType::Empty
                        || this->map[i - 1][j] == TileType::Spawn
                        || this->map[i - 1][j] == TileType::Base
                    )
                ) {
                    (*this->graph->find({i - 1, j + 1}))->link(node, sqrtOf2);
                }
            }
        }

        mapFile.close();
        this->updateMapSize();
        this->updateTiles();

        std::vector<std::vector<IPoint>> paths;
        for (IPoint& spawn : this->spawns) this->efficientPathfindToMultipleTargets(spawn, paths);
        for (std::vector<IPoint>& path : paths) this->paths.push_back(new Path(this, path));

        if (this->gameState != nullptr) {
            delete this->gameState;
            this->gameState = nullptr;
        }
        this->gameState = new GameState(
            this->renderer,
            createRect(
                this->getPosition(),
                IPoint{
                    this->getSize().x,
                    40
                }
            ),
            health,
            cash
        );
        this->enemyHandler = new EnemyHandler(
            this->renderer,
            this->getDestRect(),
            this,
            this->gameState
        );
        return this->gameState;
    }

    bool Map::placeTurret(const IPoint& index) {
        if (
            index.x < 0
            || index.x >= this->map.size()
            || index.y < 0
            || index.y >= this->map.back().size()
            || this->map[index.x][index.y] != TileType::Empty
        ) return false;
        this->map[index.x][index.y] = TileType::TurretType;
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

            this->map[index.x][index.y] = TileType::Empty;
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
        if (
            index.x < 0
            || index.x >= this->map.size()
            || index.y < 0
            || index.y >= this->map.back().size()
            || this->map[index.x][index.y] != TileType::TurretType
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

        this->map[index.x][index.y] = TileType::Empty;

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
        this->destRect = newDestRect;
        this->updateTiles();
    }

    void Map::setPosition(const IPoint& position) {
        this->Renderable::setPosition(position);
        this->updateTiles();
    }

    SDL_Rect* Map::getDestRect() const {
        SDL_Rect* rect = new SDL_Rect();
        rect->x = this->destRect->x;
        rect->y = this->destRect->y;
        rect->w = this->tileSize.x * this->map.size();
        rect->h = (this->tileSize.y * this->map.back().size()) + this->headerHeight;
        return rect;
    }

    IPoint Map::getTileIndex(IPoint position) const {
        SDL_Rect* rect = this->getDestRect();
        if (
            position.x < rect->x
            || position.y < rect->y + this->headerHeight
            || position.x > rect->x + (this->map.size() * this->tileSize.x)
            || position.y > rect->y + (this->map.front().size() * this->tileSize.y) + this->headerHeight
        ) return {-1, -1};

        return {
            (int)std::floor((double)(position.x - this->destRect->x) / (double)this->tileSize.x),
            (int)std::floor((double)(position.y - (this->destRect->y + this->headerHeight)) / (double)this->tileSize.y)
        };
    }

    IPoint Map::getTileIndexCenter(SDL_Rect* rect) const {
        return this->getTileIndex({
            rect->x + (rect->w >> 1),
            rect->y + (rect->h >> 1)
        });
    }

    SDL_Rect* Map::getTileDest(const IPoint& index) const {
        SDL_Rect* rect = new SDL_Rect();
        rect->x = this->destRect->x + (this->tileSize.x * index.x);
        rect->y = this->destRect->y + (this->tileSize.y * index.y) + this->headerHeight;
        rect->w = this->tileSize.x;
        rect->h = this->tileSize.y;
        return rect;
    }

    IPoint Map::getTileCenter(const IPoint& index) const {
        return {
            this->destRect->x + (this->tileSize.x * index.x) + (this->tileSize.x >> 1),
            this->destRect->y + (this->tileSize.y * index.y) + (this->tileSize.y >> 1) + this->headerHeight
        };
    }

    int Map::getTileType(const IPoint& index) const {
        if (index.x < 0 || index.x >= this->map.size() || index.y < 0 || index.y >= this->map.back().size()) return TileType::Wall;
        return this->map[index.x][index.y];
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
};
