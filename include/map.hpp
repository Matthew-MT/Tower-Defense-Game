#pragma once
#include <SDL2/SDL.h>
#include "game.hpp"
#include <fstream>
#include <vector>
#include <string>

namespace game {
    class Map {
    private:
        std::vector<std::vector<int>> map;
    public:
        Map(const std::string& mapFileName) {
            std::fstream mapFile(mapFileName, std::ios_base::in);
            std::string buffer;
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
    };
};
