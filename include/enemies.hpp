#pragma once
#include <SDL2/SDL.h>
#include "enemies_h.hpp"
#include "pathfinder_h.hpp"
#include "game_state.hpp"
#include <unordered_set>
#include "sprite.hpp"
#include "map_h.hpp"
#include <vector>

namespace game {
    Enemy::Enemy(
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

    Enemy::~Enemy() {}

    void Enemy::tick(double scalar) {
        IPoint next = this->path->next(scalar, this->getPosition(), this->movementSpeed);
        if (next.x != -1 && next.y != -1) this->setPosition(next);
        else {
            this->gameState->reduceHealth();
            this->handler->despawn(this);
        }
    }

    EnemyHandler::EnemyHandler(
        SDL_Renderer* initRenderer,
        SDL_Rect* initDestRect
    ) : Renderable{
        initRenderer,
        initDestRect
    } {}

    EnemyHandler::~EnemyHandler() {}

    void EnemyHandler::spawn() {}

    void EnemyHandler::despawn(Enemy* enemy) {
        this->enemies.erase(enemy);
        delete enemy;
    }
};
