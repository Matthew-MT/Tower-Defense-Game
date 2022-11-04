#pragma once
#include <SDL2/SDL.h>
#include "forward.hpp"
#include "game_state_h.hpp"
#include "enemies_h.hpp"
#include "sprite.hpp"
#include <algorithm>
#include "enums.hpp"
#include "graph.hpp"
#include "text.hpp"
#include <fstream>
#include <vector>
#include <string>

namespace game {
    class MapData {
    public:
        std::vector<std::vector<int>> map;
        std::vector<SDL_Texture*> textures;

        MapData(
            const std::vector<std::vector<int>>& initMap,
            const std::vector<SDL_Texture*>& initTextures
        );
    };

    class Map : public Renderable {
    public:
        using TileGraph = Graph<IPoint>;
    protected:
        TTF_Font* font;
        SDL_Texture* deathBackground;
        Text* deathText;
        GameState* gameState = nullptr;
        MapMenu* mapMenu = nullptr;
        TileGraph* graph;
        EnemyHandler* enemyHandler;
        TurretHandler* turretHandler;
        std::vector<std::string> mapNames;
        std::unordered_map<
            std::string,
            std::pair<
                GameState*,
                std::vector<std::vector<int>>
            >
        > maps;
        std::vector<std::vector<int>>* mapRef;
        std::vector<SDL_Texture*> textures;
        // std::vector<std::vector<StaticSprite*>> mapSprites;
        std::vector<IPoint>
            spawns,
            bases;
        std::vector<Path*> paths;
        std::string
            title,
            map;
        IPoint tileSize;
        int headerHeight;
        bool dead = false;

        // Call this function if you update tile sizes or map destRect width and height.
        virtual void updateMapSize();

        // Call this function if you update the map's position.
        virtual void updateMapPosition();

        // Call this function if you update tile sizes or map destRect width and height or the map's position.
        virtual void updateMapMenu();

        virtual bool efficientPathfindToMultipleTargets(const IPoint& origin, std::vector<std::vector<IPoint>>& paths);
    public:
        Map(
            SDL_Renderer* initRenderer,
            TTF_Font* initFont,
            SDL_Rect* initDestRect,
            const IPoint& initTileSize,
            const std::string& initTitle,
            int initHeaderHeight = 40
        );
        ~Map();

        virtual void render();
        virtual void tick(double scalar);
        virtual void handleEvent(SDL_Event* event);
        virtual GameState* loadMap(const std::string& mapFileName);
        virtual void start(Option option);
        virtual void displayDeathScreen();

        virtual bool placeTurret(const IPoint& index);
        virtual bool sellTurret(const IPoint& index);

        virtual void setDestRect(SDL_Rect* newDestRect);
        virtual void setPosition(const IPoint& position);

        virtual SDL_Rect* getMapDestRect() const;
        virtual SDL_Rect* getDestRect() const;
        virtual IPoint getSize() const;
        virtual IPoint getTileIndex(IPoint position) const;
        virtual IPoint getTileIndexCenter(SDL_Rect* rect) const;
        virtual SDL_Rect* getTileDest(const IPoint& index) const;
        virtual IPoint getTileCenter(const IPoint& index) const;
        virtual int getTileType(const IPoint& index) const;
        virtual const std::vector<IPoint>& getAllSpawns() const;
        virtual const std::vector<IPoint>& getAllBases() const;
        virtual IPoint getTileSize() const;
        virtual const std::vector<Path*>& getPaths() const;
        virtual GameState* getGameState();
        virtual EnemyHandler* getEnemyHandler();
    };
};
