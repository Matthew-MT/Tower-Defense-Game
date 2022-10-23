#pragma once
#include <SDL2/SDL.h>
#include "forward.hpp"
#include "game_state.hpp"
#include "enemies_h.hpp"
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
        GameState* gameState;
        TileGraph* graph;
        EnemyHandler* enemyHandler;
        std::vector<std::vector<int>> map;
        std::vector<SDL_Texture*> textures;
        std::vector<std::vector<StaticSprite*>> mapSprites;
        std::vector<IPoint>
            spawns,
            bases;
        std::vector<Path*> paths;

        // Call this function if you update tile sizes or map destRect width and height.
        virtual void updateMapSize();

        // Call this function if you update the map's position.
        virtual void updateTiles();

        virtual bool efficientPathfindToMultipleTargets(const IPoint& origin, std::vector<std::vector<IPoint>>& paths);
    public:
        Map(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect,
            const IPoint& initTileSize
        );
        ~Map();

        virtual void render();
        virtual void tick(double scalar);
        virtual GameState* loadMap(const std::string& mapFileName);

        virtual bool placeTurret(const IPoint& index);
        virtual bool sellTurret(const IPoint& index);

        virtual void setDestRect(SDL_Rect* newDestRect);
        virtual void setPosition(const IPoint& position);

        virtual SDL_Rect* getDestRect() const;
        virtual IPoint getTileIndex(IPoint position) const;
        virtual IPoint getTileIndexCenter(SDL_Rect* rect) const;
        virtual SDL_Rect* getTileDest(const IPoint& index) const;
        virtual IPoint getTileCenter(const IPoint& index) const;
        virtual int getTileType(const IPoint& index) const;
        virtual const std::vector<IPoint>& getAllSpawns() const;
        virtual const std::vector<IPoint>& getAllBases() const;
        virtual IPoint getTileSize() const;
        virtual const std::vector<Path*>& getPaths() const;
        virtual EnemyHandler* getEnemyHandler();
    };
};
