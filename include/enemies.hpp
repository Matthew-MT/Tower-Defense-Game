#pragma once
#include <SDL2/SDL.h>
#include "game_state.hpp"
#include "sprite.hpp"
#include "map.hpp"

namespace game {
    class Enemy : public StaticSprite {
    protected:
        GameState* gameState;
        Path* path;
        int movementSpeed;
    public:
        Enemy(
            SDL_Renderer* initRenderer,
            SDL_Texture* initTexture,
            GameState* initGameState,
            Path* initPath,
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
            movementSpeed{initMovementSpeed} {}
        ~Enemy() {}

        void tick(double scalar) {
            IPoint next = this->path->next(scalar, this->getPosition(), this->movementSpeed);
            if (next.x != -1 && next.y != -1) this->setPosition(next);
            else this->gameState->reduceHealth();
        }
    };

    class EnemyHandler : public Renderable {
    protected:
    public:
        EnemyHandler(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect
        ) : Renderable{
            initRenderer,
            initDestRect
        } {}
        ~EnemyHandler() {}
    };
};
