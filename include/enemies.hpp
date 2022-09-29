#pragma once
#include <SDL2/SDL.h>
#include "sprite.hpp"
#include "map.hpp"

namespace game {
    class Enemy : public StaticSprite {
    protected:
        Path* path = nullptr;
        int movementSpeed;
    public:
        Enemy(
            SDL_Renderer* initRenderer,
            SDL_Texture* initTexture,
            Path* initPath,
            int initMovementSpeed,
            SDL_Rect* initDestRect,
            SDL_Rect* initSourceRect = nullptr
        ) : StaticSprite{
            initRenderer,
            initTexture,
            initDestRect,
            initSourceRect
        }, path{initPath}, movementSpeed{initMovementSpeed} {}
        ~Enemy() {}

        void tick(double scalar) {
            IPoint next = this->path->next(scalar, this->getPosition(), this->movementSpeed);
            if (next.x != -1 && next.y != -1) this->setPosition(next);
            else {
            }
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
