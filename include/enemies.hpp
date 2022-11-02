#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "enemies_h.hpp"
#include "pathfinder_h.hpp"
#include "game_state.hpp"
#include "map_menu_h.hpp"
#include <unordered_set>
#include "sprite.hpp"
#include "map_h.hpp"
#include "sound.hpp"
#include <vector>

namespace game {
    EnemyData::EnemyData(
        SDL_Texture* initTexture,
        int initMovementSpeed,
        int initHealth,
        int initReward
    ) :
        texture{initTexture},
        movementSpeed{initMovementSpeed},
        health{initHealth},
        reward{initReward} {}

    Enemy::Enemy(
        SDL_Renderer* initRenderer,
        Map* initMap,
        GameState* initGameState,
        Path* initPath,
        EnemyHandler* initHandler,
        EnemyData* initData,
        SDL_Rect* initDestRect,
        SDL_Rect* initSourceRect = nullptr
    ) : StaticSprite{
        initRenderer,
        initData->texture,
        initDestRect,
        initSourceRect
    },
        gameState{initGameState},
        map{initMap},
        path{initPath},
        handler{initHandler},
        movementSpeed{initData->movementSpeed},
        health{initData->health},
        reward{initData->reward} {}

    Enemy::~Enemy() {
        for (Turret* turret : this->targetedBy) turret->stopTracking();
    }

    void Enemy::tick(double scalar) {
        DPoint
            center = this->getCenter(),
            next = this->path->next(scalar, center, this->movementSpeed);
        if (next.x != std::numeric_limits<double>::max() && next.y != std::numeric_limits<double>::max()) {
            this->setCenter(next);
        } else {
            this->gameState->reduceHealth();
            this->handler->despawn(this);
        }
    }

    void Enemy::setPath(Path* path) {
        this->path = path;
    }

    void Enemy::setCenter(const DPoint& center) {
        IPoint size = this->getSize();
        this->setPosition(DPoint{
            center.x - ((double)size.x / 2.f),
            center.y - ((double)size.y / 2.f)
        });
    }

    void Enemy::damage(int amount) {
        this->health -= amount;
        this->gameState->earn(this->reward);
        if (this->health <= 0) this->handler->despawn(this);
    }

    void Enemy::track(Turret* turret) {
        this->targetedBy.push_back(turret);
    }

    DPoint Enemy::getCenter() {
        return {
            this->position.x + ((double)this->destRect->w / 2.f),
            this->position.y + ((double)this->destRect->h / 2.f)
        };
    }

    EnemyHandler::EnemyHandler(
        SDL_Renderer* initRenderer,
        SDL_Rect* initDestRect,
        Map* initMap,
        GameState* initGameState
    ) :
        Renderable{
            initRenderer,
            initDestRect
        },
        gameState{initGameState},
        map{initMap} {
        std::fstream
            textureAssociation("assets/config/enemy_data_association.txt", std::ios_base::in);
        std::string buffer;

        while (!textureAssociation.eof()) {
            std::getline(textureAssociation, buffer);
            std::fstream
                enemyData("assets/enemies/" + buffer, std::ios_base::in);

            while (!enemyData.eof()) {
                std::getline(enemyData, buffer);
                SDL_Surface* surface = IMG_Load(((std::string)"assets/images/" + buffer).c_str());
                std::getline(enemyData, buffer);
                int movementSpeed = std::stoi(buffer);
                std::getline(enemyData, buffer);
                int health = std::stoi(buffer);
                std::getline(enemyData, buffer);
                int reward = std::stoi(buffer);
                this->types.push_back(new EnemyData(
                    SDL_CreateTextureFromSurface(
                        this->renderer,
                        surface
                    ),
                    movementSpeed,
                    health,
                    reward
                ));
                SDL_FreeSurface(surface);
            }
        }

        textureAssociation.close();
        // this->spawn();
    }

    EnemyHandler::~EnemyHandler() {}

    void EnemyHandler::render() {
        for (Enemy* enemy : this->enemies) enemy->render();
    }

    void EnemyHandler::tick(double scalar) {
        for (Enemy* enemy : this->enemies) enemy->tick(scalar);
        if (std::rand() % 1000 < 4) {
            std::vector<IPoint> spawns = this->map->getAllSpawns();
            this->spawn(0, spawns[std::rand() % spawns.size()]);
        }
        for (Enemy* enemy : this->dying) {
            this->enemies.erase(enemy);
            delete enemy;
        }
        this->dying.clear();
    }

    void EnemyHandler::start(MapMenu::Option option) {}

    typename EnemyHandler::Enemies::iterator EnemyHandler::begin() {
        return this->enemies.begin();
    }

    typename EnemyHandler::Enemies::iterator EnemyHandler::end() {
        return this->enemies.end();
    }

    void EnemyHandler::spawn(int type, IPoint index) {
        SDL_Rect* initDestRect = this->map->getTileDest(index);

        std::vector<Path*> validPaths;
        for (Path* path : this->map->getPaths()) if (path->isIndexInPath(index)) validPaths.push_back(path);
        this->enemies.insert(new Enemy(
            this->renderer,
            this->map,
            this->gameState,
            validPaths[std::rand() % validPaths.size()],
            this,
            this->types.at(type),
            initDestRect
        ));
    }

    void EnemyHandler::despawn(Enemy* enemy) {
        this->dying.insert(enemy);
    }
};
