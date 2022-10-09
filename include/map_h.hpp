#pragma once
#include <SDL2/SDL.h>
#include "forward.hpp"
#include "game_state.hpp"
#include "sprite.hpp"
#include <algorithm>
#include "enums.hpp"
#include "graph.hpp"
#include <fstream>
#include <vector>
#include <string>

namespace game {
    class Map : public Renderable {
    public:
        using TileGraph = Graph<IPoint>;
    protected:
        IPoint tileSize;
        TileGraph* graph;
        Pathfinder* pathfinder;
        std::vector<std::vector<int>> map;
        std::vector<SDL_Texture*> textures;
        std::vector<std::vector<StaticSprite*>> mapSprites;
        std::vector<IPoint>
            spawns,
            bases;

        // Call this function if you update tile sizes or map destRect width and height.
        void updateMapSize();

        // Call this function if you update the map's position.
        void updateTiles();
    public:
        Map(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect,
            const IPoint& initTileSize
        );
        ~Map();

        void render();
        GameState* loadMap(const std::string& mapFileName);

        bool placeTurret(const IPoint& index);
        bool sellTurret(const IPoint& index);

        void setDestRect(SDL_Rect* newDestRect);
        void setPosition(const IPoint& position);

        SDL_Rect* getDestRect() const;
        IPoint getTileIndex(IPoint position) const;
        IPoint getTileIndexCenter(SDL_Rect* rect) const;
        SDL_Rect* getTileDest(const IPoint& index) const;
        IPoint getTileCenter(const IPoint& index) const;
        int getTileType(const IPoint& index) const;
        const std::vector<IPoint>& getAllSpawns() const;
        const std::vector<IPoint>& getAllBases() const;
    };
};
