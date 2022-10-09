#pragma once
#include <SDL2/SDL.h>
#include "forward.hpp"
#include <unordered_set>
#include "sprite.hpp"
#include <vector>

namespace game {
    class Enemy : public StaticSprite {
    protected:
        GameState* gameState;
        Path* path;
        EnemyHandler* handler;
        int movementSpeed;
    public:
        Enemy(
            SDL_Renderer* initRenderer,
            SDL_Texture* initTexture,
            GameState* initGameState,
            Path* initPath,
            EnemyHandler* initHandler,
            int initMovementSpeed,
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
        Enemies enemies;
        std::vector<Path*> paths;
    public:
        EnemyHandler(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect
        );

        ~EnemyHandler();

        void render();

        typename Enemies::iterator begin();
        typename Enemies::iterator end();

        void spawn();
        void despawn(Enemy* enemy);
    };
};
