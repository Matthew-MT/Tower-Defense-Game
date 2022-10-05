#pragma once
#include <SDL2/SDL.h>
#include "game_state.hpp"
#include <unordered_set>
#include "sprite.hpp"
#include "map.hpp"
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
            SDL_Rect* initSourceRect = nullptr
        ) : StaticSprite{
            initRenderer,
            initTexture,
            initDestRect,
            initSourceRect
        },
            gameState{initGameState},
            path{initPath},
            handler{initHandler},
            movementSpeed{initMovementSpeed} {}

        ~Enemy() {}

        void tick(double scalar) {
            IPoint next = this->path->next(scalar, this->getPosition(), this->movementSpeed);
            if (next.x != -1 && next.y != -1) this->setPosition(next);
            else {
                this->gameState->reduceHealth();
                this->handler->despawn(this);
            }
        }
    };

    class EnemyHandler : public Renderable {
    protected:
        Map* map;
        std::unordered_set<Enemy*> enemies;
        std::vector<Path*> paths;
    public:
        EnemyHandler(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect
        ) : Renderable{
            initRenderer,
            initDestRect
        } {}

        ~EnemyHandler() {}

        void spawn() {}

        void despawn(Enemy* enemy) {
            this->enemies.erase(enemy);
            delete enemy;
        }
    };
};
