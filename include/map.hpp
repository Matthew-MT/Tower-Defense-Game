#pragma once
#include <SDL2/SDL.h>
#include "map_h.hpp"
#include "pathfinder.hpp"
#include "game_state.hpp"
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
            this->destRect->w == -1
        ) {
            this->destRect->w
                = this->tileSize.x
                * this->map.size();
        } else {
            this->tileSize.x
                = this->destRect->w
                / this->map.size();
        }

        if (
            this->destRect->h == -1
        ) {
            this->destRect->h
                = this->tileSize.y
                * this->map.back().size();
        } else {
            this->tileSize.y
                = this->destRect->h
                / this->map.back().size();
        }
    }

    void Map::updateTiles() {
        for (int i = 0; i < this->mapSprites.size(); i++) {
            for (int j = 0; j < this->mapSprites[i].size(); j++) {
                this->mapSprites[i][j]->setDestRect(this->getTileDest({i, j}));
            }
        }
    }

    Map::Map(
        SDL_Renderer* initRenderer,
        SDL_Rect* initDestRect,
        const IPoint& initTileSize
    ) :
        Renderable{
            initRenderer,
            initDestRect
        },
        tileSize{initTileSize} {
        this->graph = new TileGraph();

        std::fstream
            textureAssociation("assets/config/texture_association.txt", std::ios_base::in);
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
        for (SDL_Texture* texture : this->textures) SDL_DestroyTexture(texture);
        for (std::vector<StaticSprite*>& col : this->mapSprites) for (StaticSprite* sprite : col) delete sprite;
    }

    void Map::render() {
        for (int i = 0; i < this->map.size(); i++) for (int j = 0; j < this->map[i].size(); j++) {
            SDL_RenderCopy(
                this->renderer,
                this->textures.at(this->map[i][j]),
                nullptr,
                this->mapSprites[i][j]->getDestRect()
            );
        }
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

                if (type == TileType::Spawn) this->spawns.push_back(index);
                else if (type == TileType::Base) this->bases.push_back(index);

                if (type != TileType::Empty) continue;
                std::pair<TileGraph::Nodes::iterator, bool> r = this->graph->insert(index);
                TileGraph::Node* node = *r.first;

                if (i > 0 && this->map[i - 1][j] == TileType::Empty) {
                    (*this->graph->find({i - 1, j}))->link(node);
                }
                if (j > 0 && this->map[i][j - 1] == TileType::Empty) {
                    (*this->graph->find({i, j - 1}))->link(node);
                }
                if (i > 0 && j > 0 && this->map[i - 1][j - 1] == TileType::Empty) {
                    (*this->graph->find({i - 1, j - 1}))->link(node, 1.41421356);
                }
            }
        }

        mapFile.close();
        this->updateMapSize();
        this->updateTiles();

        return new GameState(health, cash);
    }

    bool Map::placeTurret(const IPoint& index) {
        if (this->map[index.x][index.y] != TileType::Empty) return false;
        TileGraph::Node* node = *this->graph->find(index);
        TileGraph::Node::Neighbors neighbors = node->getNeighbors();
        this->graph->erase(node);
        std::vector<std::vector<IPoint>> paths;
        for (IPoint& spawn : this->spawns) {
            for (const IPoint& base : this->bases) {
                std::vector<TileGraph::Node*> foundPath = this->graph->aStar(
                    *this->graph->find(spawn),
                    *this->graph->find(spawn),
                    [&](TileGraph::Node* a, TileGraph::Node* b) -> bool {
                        return distance(*a->getValue(), *b->getValue());
                    }
                );
                if (foundPath.size() == 0) {
                    this->graph->insert(node);
                    for (TileGraph::Node* neighbor : neighbors) node->link(neighbor);
                    return false;
                }
                std::vector<IPoint> path;
                for (TileGraph::Node* node : foundPath) path.push_back(*node->getValue());
                paths.push_back(path);
            }
        }
        return true;
    }

    bool Map::sellTurret(const IPoint& index) {
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
        rect->h = this->tileSize.y * this->map.back().size();
        return rect;
    }

    IPoint Map::getTileIndex(IPoint position) const {
        if (
            position.x < 0
            || position.y < 0
            || position.x > this->map.size() * this->tileSize.x
            || position.y > this->map.front().size() * this->tileSize.y
        ) return {-1, -1};

        return {
            (int)std::floor((double)(position.x - this->destRect->x) / (double)this->tileSize.x),
            (int)std::floor((double)(position.y - this->destRect->y) / (double)this->tileSize.y)
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
        rect->y = this->destRect->y + (this->tileSize.y * index.y);
        rect->w = this->tileSize.x;
        rect->h = this->tileSize.y;
        return rect;
    }

    IPoint Map::getTileCenter(const IPoint& index) const {
        return {
            this->destRect->x + (this->tileSize.x * index.x) + (this->tileSize.x >> 1),
            this->destRect->y + (this->tileSize.y * index.y) + (this->tileSize.y >> 1)
        };
    }

    int Map::getTileType(const IPoint& index) const {
        return this->map[index.x][index.y];
    }

    const std::vector<IPoint>& Map::getAllSpawns() const {
        return this->spawns;
    }

    const std::vector<IPoint>& Map::getAllBases() const {
        return this->bases;
    }
};
