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
    EnemyData::EnemyData(int initMovementSpeed, int initHealth) : movementSpeed{initMovementSpeed}, health{initHealth} {}

    Enemy::Enemy(
        SDL_Renderer* initRenderer,
        SDL_Texture* initTexture,
        GameState* initGameState,
        Path* initPath,
        EnemyHandler* initHandler,
        EnemyData* initData,
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
        movementSpeed{initData->movementSpeed},
        health{initData->health} {}

    Enemy::~Enemy() {}

    void Enemy::tick(double scalar) {
        IPoint next = this->path->next(scalar, this->getPosition(), this->movementSpeed);
        if (next.x != -1 && next.y != -1) this->setPosition(next);
        else {
            this->gameState->reduceHealth();
            this->handler->despawn(this);
        }
    }

    void Enemy::setPath(Path* path) {
        delete this->path;
        this->path = path;
    }

    IPoint Enemy::getCenter() {
        return {
            this->destRect->x + (this->destRect->w >> 1),
            this->destRect->y + (this->destRect->h >> 1)
        };
    }

    EnemyHandler::EnemyHandler(
        SDL_Renderer* initRenderer,
        SDL_Rect* initDestRect
    ) : Renderable{
        initRenderer,
        initDestRect
    } {
        this->spawn();
    }

    EnemyHandler::~EnemyHandler() {}

    void EnemyHandler::render() {
        for (Enemy* enemy : this->enemies) enemy->render();
    }

    typename EnemyHandler::Enemies::iterator EnemyHandler::begin() {
        return this->enemies.begin();
    }

    typename EnemyHandler::Enemies::iterator EnemyHandler::end() {
        return this->enemies.end();
    }

    void EnemyHandler::spawn() {
    }

    void EnemyHandler::despawn(Enemy* enemy) {
        this->enemies.erase(enemy);
        delete enemy;
    }
};
