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
    EnemyData::EnemyData(
        SDL_Texture* initTexture,
        int initMovementSpeed,
        int initHealth
    ) :
        texture{initTexture},
        movementSpeed{initMovementSpeed},
        health{initHealth} {}

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
        SDL_Rect* initDestRect,
        GameState* initGameState
    ) :
        Renderable{
            initRenderer,
            initDestRect
        },
        gameState{initGameState} {
        std::fstream
            textureAssociation("assets/config/enemy_texture_association.txt", std::ios_base::in);
        std::string buffer;

        while (!textureAssociation.eof()) {
            std::getline(textureAssociation, buffer);
            SDL_Surface* surface = SDL_LoadBMP(((std::string)"assets/images/" + buffer).c_str());
            this->types.push_back(new EnemyData(
                SDL_CreateTextureFromSurface(
                    this->renderer,
                    surface
                ),
                10,
                10
            ));
            SDL_FreeSurface(surface);
        }

        textureAssociation.close();
        // this->spawn();
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

    void EnemyHandler::spawn(int type, IPoint index) {
        IPoint
            origin = this->map->getTileCenter(index),
            tileSize = this->map->getTileSize();
        SDL_Rect* initDestRect;
        initDestRect->x = origin.x;
        initDestRect->y = origin.y;
        initDestRect->w = tileSize.x;
        initDestRect->h = tileSize.y;
        std::vector<Path*> validPaths;
        for (Path* path : this->paths) if (path->isIndexInPath(index)) validPaths.push_back(path);
        this->enemies.insert(new Enemy(
            this->renderer,
            this->types.at(type)->texture,
            this->gameState,
            validPaths[std::rand() % validPaths.size()],
            this,
            this->types.at(type),
            initDestRect
        ));
    }

    void EnemyHandler::despawn(Enemy* enemy) {
        this->enemies.erase(enemy);
        delete enemy;
    }
};
