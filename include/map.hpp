#pragma once
#include <SDL2/SDL.h>
#include "game_state.hpp"
#include "sprite.hpp"
#include <algorithm>
#include <fstream>
#include <vector>
#include <string>

namespace game {
    double distance(IPoint a, IPoint b) {
        return std::sqrt(
            std::pow(a.x - b.x, 2)
            + std::pow(a.y - b.y, 2)
        );
    }

    double distanceCenter(SDL_Rect* a, SDL_Rect* b) {
        return distance(
            {
                a->x + (a->w >> 1),
                a->y + (a->h >> 1)
            },
            {
                b->x + (b->w >> 1),
                b->y + (b->h >> 1)
            }
        );
    }

    class Map : public Renderable {
    protected:
        IPoint tileSize;
        std::vector<std::vector<int>> map;
        std::vector<SDL_Texture*> textures;
        std::vector<std::vector<StaticSprite*>> mapSprites;

        // Call this function if you update tile sizes or map destRect width and height.
        void updateMapSize() {
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

        // Call this function if you update the map's position.
        void updateTiles() {
            for (int i = 0; i < this->mapSprites.size(); i++) {
                for (int j = 0; j < this->mapSprites[i].size(); j++) {
                    this->mapSprites[i][j]->setDestRect(this->getTileDest({i, j}));
                }
            }
        }
    public:
        Map(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect,
            const IPoint& initTileSize
        ) :
            Renderable{
                initRenderer,
                initDestRect
            },
            tileSize{initTileSize} {
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

        ~Map() {
            for (SDL_Texture* texture : this->textures) SDL_DestroyTexture(texture);
            for (std::vector<StaticSprite*>& col : this->mapSprites) for (StaticSprite* sprite : col) delete sprite;
        }

        void render() {
            for (int i = 0; i < this->map.size(); i++) for (int j = 0; j < this->map[i].size(); j++) {
                SDL_RenderCopy(
                    this->renderer,
                    this->textures.at(this->map[i][j]),
                    nullptr,
                    this->mapSprites[i][j]->getDestRect()
                );
            }
        }

        void loadMap(const std::string& mapFileName) {
            std::fstream
                mapFile("assets/maps/" + mapFileName, std::ios_base::in);
            std::string buffer;

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
                    this->mapSprites.back().push_back(new StaticSprite(
                        this->renderer,
                        this->textures.at(this->map[i][j]),
                        this->getTileDest({i, j})
                    ));
                }
            }

            mapFile.close();
            this->updateMapSize();
            this->updateTiles();
        }

        void setDestRect(SDL_Rect* newDestRect) {
            this->destRect = newDestRect;
            this->updateTiles();
        }

        void setPosition(const IPoint& position) {
            this->destRect->x = position.x;
            this->destRect->y = position.y;
            this->updateTiles();
        }

        SDL_Rect* getDestRect() const {
            SDL_Rect* rect = new SDL_Rect();
            rect->x = this->destRect->x;
            rect->y = this->destRect->y;
            rect->w = this->tileSize.x * this->map.size();
            rect->h = this->tileSize.y * this->map.back().size();
            return rect;
        }

        IPoint getTileIndex(IPoint position) const {
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

        IPoint getTileIndexCenter(SDL_Rect* rect) const {
            return this->getTileIndex({
                rect->x + (rect->w >> 1),
                rect->y + (rect->h >> 1)
            });
        }

        SDL_Rect* getTileDest(const IPoint& index) const {
            SDL_Rect* rect = new SDL_Rect();
            rect->x = this->destRect->x + (this->tileSize.x * index.x);
            rect->y = this->destRect->y + (this->tileSize.y * index.y);
            rect->w = this->tileSize.x;
            rect->h = this->tileSize.y;
            return rect;
        }

        IPoint getTileCenter(const IPoint& index) const {
            return {
                this->destRect->x + (this->tileSize.x * index.x) + (this->tileSize.x >> 1),
                this->destRect->y + (this->tileSize.y * index.y) + (this->tileSize.y >> 1)
            };
        }

        int getTileType(const IPoint& index) const {
            return this->map[index.x][index.y];
        }
    };

    class Path {
    protected:
        Map map;
        std::vector<IPoint> path;
    public:
        Path(const Map& initMap, const std::vector<IPoint>& initPath) : map{initMap}, path{initPath} {}
        ~Path() {}

        IPoint next(double scalar, IPoint currentPosition, int movementSpeed) {
            if (movementSpeed < 0) throw "Positive movement speed only.";

            IPoint index = this->map.getTileIndex(currentPosition);
            std::vector<IPoint>::iterator i;

            if (index.x == -1 && index.y == -1) {
                std::vector<IPoint>::reverse_iterator r = std::find_if(
                    this->path.rbegin(),
                    this->path.rend(),
                    [&](const IPoint& point) -> bool {
                        return
                            point.x <= index.x + 1
                            && point.x >= index.x - 1
                            && point.y <= index.x + 1
                            && point.y >= index.y - 1;
                    }
                );

                if (r == this->path.rend()) throw "Pathfinders must receive a new path when paths are recomputed.";
                i = (++r).base();
            } else {
                i = std::find_if(
                    this->path.begin(),
                    this->path.end(),
                    [&](const IPoint& point) -> bool {
                        return point.x == index.x && point.y == index.y;
                    }
                );

                if (i == this->path.end()) throw "Pathfinders must receive a new path when paths are recomputed.";
                if (++i == this->path.end()) return {-1, -1};
            }

            double range = (double)movementSpeed * scalar;
            IPoint targetPosition = this->map.getTileCenter(*i);
            IPoint slope = {
                currentPosition.x - targetPosition.x,
                currentPosition.y - targetPosition.y
            };
            double normalizeScalar = std::sqrt(std::pow((double)slope.x, 2) + std::pow((double)slope.y, 2)) / range;

            slope.x = (int)(slope.x / normalizeScalar);
            slope.y = (int)(slope.y / normalizeScalar);
            return {
                currentPosition.x + slope.x,
                currentPosition.y + slope.y
            };
        }
    };
};
