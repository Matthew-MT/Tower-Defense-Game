#pragma once
#include <SDL2/SDL.h>
#include <SDL_image.h>
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
        int initReward,
        int initSheild
        
    ) :
        texture{initTexture},
        movementSpeed{initMovementSpeed},
        health{initHealth},
        reward{initReward},
        sheild{initSheild} {}

    Enemy::Enemy(
        SDL_Renderer* initRenderer,
        Map* initMap,
        GameState* initGameState,
        Path* initPath,
        EnemyHandler* initHandler,
        EnemyData* initData,
        double initDifficulty,
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
        health{(int)std::round((double)initData->health * initDifficulty)},
        sheild{(int)std::round((double)initData->sheild * initDifficulty)},
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
        if (this->health > 0) {
            if(this->sheild >0){
                this->sheild -= amount;
            }else{
                this->health -= amount;
                if (this->health <= 0) {
                    this->gameState->earn(this->reward);
                    this->handler->despawn(this);
                }
            }
        }
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

    WaveData::WaveData(
        int initTime,
        int initSpawnedPerSec,
        const std::vector<int>& initWaveList
    ) :
        time{initTime},
        spawnedPerSec{initSpawnedPerSec},
        waveList{initWaveList} {}

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
            enemyDataAssociation("assets/config/enemy_data_association.txt", std::ios_base::in),
            difficultyData("assets/config/wave_difficulty.txt", std::ios_base::in);
        std::string buffer;

        while (!enemyDataAssociation.eof()) {
            std::getline(enemyDataAssociation, buffer);
            std::fstream
                enemyData("assets/enemies/" + buffer, std::ios_base::in);

            while (!enemyData.eof()) {
                std::getline(enemyData, buffer);
                SDL_Surface* surface = IMG_Load(((std::string)"assets/images/enemies/" + buffer).c_str());
                std::getline(enemyData, buffer);
                int movementSpeed = std::stoi(buffer);
                std::getline(enemyData, buffer);
                int health = std::stoi(buffer);
                std::getline(enemyData, buffer);
                int reward = std::stoi(buffer);
                std::getline(EnemyData, buffer);
                int sheild = std::stoi(buffer);
                this->types.push_back(new EnemyData(
                    SDL_CreateTextureFromSurface(
                        this->renderer,
                        surface
                    ),
                    movementSpeed,
                    health,
                    reward,
                    sheild
                ));
                SDL_FreeSurface(surface);
            }

            enemyData.close();
        }
        enemyDataAssociation.close();

        while (!difficultyData.eof()) {
            std::getline(difficultyData, buffer);
            this->difficulties.push_back(std::stod(buffer));
        }
        difficultyData.close();
    }

    EnemyHandler::~EnemyHandler() {}

    void EnemyHandler::render() {
        for (Enemy* enemy : this->enemies) enemy->render();
    }

    void EnemyHandler::tick(double scalar) {
        if (!this->started) return;
        for (Enemy* enemy : this->enemies) enemy->tick(scalar);
        for (Enemy* enemy : this->dying) {
            this->enemies.erase(enemy);
            delete enemy;
        }
        this->dying.clear();
        this->elapsed += scalar;

        if (this->elapsed >= this->completedWavesTime + this->waves.at(this->completedWaves)->time) {
            this->completedWaves++;
            this->spawnedEnemiesTracker = 0;
            if (this->completedWaves >= this->waves.size()) {
                this->elapsed -= (double)this->completedWavesTime;
                this->completedWaves = 0;
                this->completedWavesTime = 0;
                this->spawnedEnemiesTracker = 0;
            } else this->completedWavesTime += this->waves.at(this->completedWaves - 1)->time;
        }

        if (this->completedWaves >= this->waves.size()) {
            if (this->elapsed > this->completedWavesTime) this->elapsed -= (double)this->completedWavesTime;
            this->completedWaves = 0;
            this->spawnedEnemiesTracker = 0;
            this->completedWavesTime = 0;
        }

        WaveData* wave = this->waves.at(this->completedWaves);
        if (this->spawnedEnemiesTracker < wave->waveList.size()) {
            double inWaveTime = this->elapsed - (double)this->completedWavesTime;
            int spawnedEnemies = (int)std::round(inWaveTime * wave->spawnedPerSec) - this->spawnedEnemiesTracker;
            std::vector<IPoint> spawns = this->map->getAllSpawns();
            for (
                int i = this->spawnedEnemiesTracker, e = this->spawnedEnemiesTracker + spawnedEnemies;
                i < e; i++
            ) {
                if (this->spawnedEnemiesTracker >= wave->waveList.size()) break;
                this->spawnedEnemiesTracker++;
                this->spawn(wave->waveList.at(i), spawns.at(std::rand() % spawns.size()));
            }
        }
    }

    void EnemyHandler::start(Option option) {
        std::string
            wavesFile,
            buffer;
        if (option == Option::Easy) {
            this->difficulty = 0;
            wavesFile = "easy.txt";
        } else if (option == Option::Normal) {
            this->difficulty = 1;
            wavesFile = "normal.txt";
        } else if (option == Option::Hard) {
            this->difficulty = 2;
            wavesFile = "hard.txt";
        } else if (option == Option::Fun) {
            this->difficulty = 3;
            wavesFile = "fun.txt";
        } else return;
        std::fstream
            wavesData(((std::string)"assets/waves/" + wavesFile).c_str(), std::ios_base::in);
        
        while (!wavesData.eof()) {
            std::getline(wavesData, buffer);
            int time = std::stoi(buffer);
            std::getline(wavesData, buffer);
            int spawnedPerSec = std::stoi(buffer);
            std::getline(wavesData, buffer);
            std::vector<int> wave;
            if (buffer.size()) wave.push_back(0);
            for (char c : buffer) {
                if (c == ',') wave.push_back(0);
                else if (c >= '0' && c <= '9') (wave.back() *= 10) += (int)(c - '0');
                else {
                    SDL_Log("`EnemyHandler` failed parsing a waves file. Check that syntax is correct in the file.");
                    throw 1;
                }
            }
            this->waves.push_back(new WaveData(
                time,
                spawnedPerSec,
                wave
            ));
        }
        this->started = true;
    }

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
            this->difficulties.at(this->difficulty),
            initDestRect
        ));
    }

    void EnemyHandler::despawn(Enemy* enemy) {
        this->dying.insert(enemy);
    }
};
