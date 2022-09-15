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
        std::vector<std::vector<int>> map;
        std::vector<SDL_Texture*> textures;
        std::vector<std::vector<StaticSprite>> mapTextures;
    public:
        Map(
            SDL_Renderer* initRenderer,
            const std::string& mapFileName
        ) : renderer{initRenderer} {
            std::fstream
                mapFile(mapFileName, std::ios_base::in),
                textureAssociation("../assets/config/texture_association.txt", std::ios_base::in);
            std::string buffer;

            while (!textureAssociation.eof()) {
                std::getline(textureAssociation, buffer);
                SDL_Texture* texture = SDL_CreateTextureFromSurface(this->renderer, SDL_LoadBMP(buffer.c_str()));
                this->textures.push_back(texture);
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
        }

        ~Map() {}

        void render() {
            for (std::vector<int>& row : this->map) for (int t : row) SDL_RenderCopy(
                this->renderer,
                this->textures.at(t)
            );
        }
    };
};
