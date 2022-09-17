#pragma once
#include <SDL2/SDL.h>
#include "sprite.hpp"
#include "game.hpp"
#include <fstream>
#include <vector>
#include <string>

namespace game {
    class Map {
    private:
        SDL_Renderer* renderer;
        SDL_Rect* destRect = nullptr;
        IPoint tileSize;
        std::vector<std::vector<int>> map;
        std::vector<SDL_Texture*> textures;
        std::vector<std::vector<StaticSprite>> mapSprites;

        void updateTiles() {
            for (int i = 0; i < this->mapSprites.size(); i++) {
                for (int j = 0; j < this->mapSprites[i].size(); j++) {
                    this->mapSprites[i][j].setDestRect(this->getTileDest({i, j}));
                }
            }
        }
    public:
        Map(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect,
            const IPoint& initTileSize,
            const std::string& mapFileName
        ) :
            renderer{initRenderer},
            destRect{initDestRect},
            tileSize{initTileSize} {
            std::fstream
                textureAssociation("../assets/config/texture_association.txt", std::ios_base::in),
                mapFile(mapFileName, std::ios_base::in);
            std::string buffer;

            while (!textureAssociation.eof()) {
                std::getline(textureAssociation, buffer);
                this->textures.push_back(
                    SDL_CreateTextureFromSurface(
                        this->renderer,
                        SDL_LoadBMP(buffer.c_str())
                    )
                );
            }

            while (!mapFile.eof()) {
                std::getline(mapFile, buffer);
                map.push_back({});
                for (char c : buffer) {
                    if (c == ',') map.back().push_back(0);
                    else if (c >= '0' && c <= '9') (map.back().back() *= 10) += (int)(c - '0');
                    else throw "Failed parsing a map file. Check that syntax is correct in the file.";
                }
            }

            for (int i = 0; i < this->map.size(); i++) {
                this->mapSprites.push_back({});
                for (int j = 0; j < this->map[i].size(); j++) {
                    this->mapSprites.back().push_back({
                        this->renderer,
                        this->textures.at(this->map[i][j]),
                        nullptr,
                        this->getTileDest({i, j})
                    });
                }
            }
        }

        ~Map() {
            for (SDL_Texture* texture : this->textures) SDL_DestroyTexture(texture);
        }

        void render() {
            for (std::vector<int>& row : this->map) for (int t : row) SDL_RenderCopy(
                this->renderer,
                this->textures.at(t),
                nullptr,
                this->destRect
            );
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

        IPoint getTileIndex(IPoint position) {
            if (
                position.x < 0
                || position.y < 0
                || position.x > this->map.size() * this->tileSize.x
                || position.y > this->map.front().size() * this->tileSize.y
            ) return {-1, -1};

            return {
                (int)std::floor((double)position.x / (double)this->tileSize.x),
                (int)std::floor((double)position.y / (double)this->tileSize.y)
            };
        }

        SDL_Rect* getTileDest(const IPoint& index) {
            SDL_Rect* rect;
            rect->x = this->destRect->x + (this->tileSize.x * index.x);
            rect->y = this->destRect->y + (this->tileSize.y * index.y);
            rect->w = this->tileSize.x;
            rect->h = this->tileSize.y;
            return rect;
        }
    };
};
