#pragma once
#include <SDL2/SDL.h>
#include <unordered_set>
#include "turret_h.hpp"
#include "forward.hpp"
#include "sprite.hpp"
#include "enums.hpp"
#include <vector>

namespace game {
    class EnemyData {
    public:
        EnemyData(SDL_Texture* initTexture, int initMovementSpeed, int initHealth, int initReward);

        SDL_Texture* texture;
        int
            movementSpeed,
            health,
            reward,
            sheild;
    };

    class Enemy : public StaticSprite {
    protected:
        Map* map;
        GameState* gameState;
        Path* path;
        EnemyHandler* handler;
        std::vector<Turret*> targetedBy;
        int
            movementSpeed,
            health,
            reward,
            sheild;
    public:
        Enemy(
            SDL_Renderer* initRenderer,
            Map* initMap,
            GameState* initGameState,
            Path* initPath,
            EnemyHandler* initHandler,
            EnemyData* initData,
            double initDifficulty,
            SDL_Rect* initDestRect,
            SDL_Rect* initSourceRect
        );

        ~Enemy();

        virtual void tick(double scalar);

        virtual void setPath(Path* path);
        virtual void setCenter(const DPoint& center);
        virtual void damage(int amount);
        virtual void track(Turret* turret);

        virtual DPoint getCenter();
    };

    class WaveData {
    public:
        WaveData(int initTime, int initSpawnedPerSec, const std::vector<int>& initWaveList);

        std::vector<int> waveList;
        int
            time,
            spawnedPerSec;
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
        std::vector<WaveData*> waves;
        std::vector<double> difficulties;
        int
            difficulty = 0,
            completedWaves = 0,
            completedWavesTime = 0,
            spawnedEnemiesTracker = 0;
        double elapsed = 0.f;
        bool started = false;
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

        virtual void start(Option option);

        virtual typename Enemies::iterator begin();
        virtual typename Enemies::iterator end();

        virtual void spawn(int type, IPoint origin);
        virtual void despawn(Enemy* enemy);
    };
};
