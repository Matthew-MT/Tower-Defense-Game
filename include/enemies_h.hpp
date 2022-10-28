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

        virtual void tick(double scalar);

        virtual void setPath(Path* path);
        virtual void setCenter(const DPoint& center);
        virtual void damage(int amount);

        virtual DPoint getCenter();
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

        virtual void render();
        virtual void tick(double scalar);

        virtual typename Enemies::iterator begin();
        virtual typename Enemies::iterator end();

        virtual void spawn(int type, IPoint origin);
        virtual void despawn(Enemy* enemy);
    };
};
