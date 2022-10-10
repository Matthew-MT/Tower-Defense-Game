#pragma once
#include <SDL2/SDL.h>
#include "forward.hpp"
#include <unordered_set>
#include "sprite.hpp"
#include <vector>

namespace game {
    class EnemyData {
    public:
        EnemyData(SDL_Texture* initTexture, int initMovementSpeed, int initHealth);

        SDL_Texture* texture;
        int
            movementSpeed,
            health;
    };

    class Enemy : public StaticSprite {
    protected:
        Map* map;
        GameState* gameState;
        Path* path;
        EnemyHandler* handler;
        int
            movementSpeed,
            health;
    public:
        Enemy(
            SDL_Renderer* initRenderer,
            SDL_Texture* initTexture,
            Map* initMap,
            GameState* initGameState,
            Path* initPath,
            EnemyHandler* initHandler,
            EnemyData* initData,
            SDL_Rect* initDestRect,
            SDL_Rect* initSourceRect
        );

        ~Enemy();

        void tick(double scalar);

        void setPath(Path* path);

        IPoint getCenter();
    };

    class EnemyHandler : public Renderable {
    public:
        using Enemies = std::unordered_set<Enemy*>;
    protected:
        Map* map;
        GameState* gameState;
        Enemies
            enemies,
            dying;
        std::vector<EnemyData*> types;
    public:
        EnemyHandler(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect,
            Map* initMap,
            GameState* initGameState
        );

        ~EnemyHandler();

        void render();
        void tick(double scalar);

        typename Enemies::iterator begin();
        typename Enemies::iterator end();

        void spawn(int type, IPoint origin);
        void despawn(Enemy* enemy);
    };
};
