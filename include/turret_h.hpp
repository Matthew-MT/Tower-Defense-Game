#pragma once
#include <SDL2/SDL.h>
#include "game_state.hpp"
#include "forward.hpp"
#include "sprite.hpp"
#include "enums.hpp"
#include "map.hpp"
#include <vector>

namespace game{
    class Turret : public Sprite
    {
        protected:
        GameState* gamestate;
        int damage;
        double reloadTime;
        int range;
        IPoint index;
        TurretHandler* turretHandler;
        Enemy* targetedEnemy = nullptr;
        double remainingReload=0;
        double angle;
        Sound* spawnSound;
        Sound* shootSound;
        public:
        Turret(
            SDL_Renderer* initRenderer,
            SDL_Texture* initTexture,
            SDL_Rect* initDestRect,
            SDL_Rect* initSourceRect,
            int initDamage,
            double initReload,
            int initRange,
            IPoint initIndex,
            TurretHandler* initTurretHandler,
            double initAngle,
            Sound* initSpawnSound,
            Sound* initShootSound
        );

        IPoint getIndex();

        
        void checkTarget(double scalar);
        void findTarget();
        void stopTracking();

        void tick(double scalar);
        void rotateTurret(DPoint enemy, DPoint turret);
        DPoint getCenter();

        ~Turret();
    };

    class TurretHandler : public Renderable
    {
        protected:
        Map* map;
        std::unordered_set<Turret*> turrets;
        std::vector<TurretData*>  turretTypes;
        bool started = false;
        public:
        TurretHandler(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect,
            Map* initMap
            ); 

        void render();

        void readTurretData(const std::string& turretFileName);

        void createTurret(int type, const IPoint& index);

        void handleEvent(SDL_Event* event);

        void tick(double scalar);

        void start(Option option);

        EnemyHandler* getEnemyHandler();

        Map* getMap();
    };

    class TurretData
    {
        public:
        int damage;
        double reload;
        int range;
        SDL_Texture* texture;
        Sound* turretSpawnSound;
        Sound* turretShootSound;
        public:
        TurretData(
            int initDamage, 
            double initReload, 
            int initRange,
            SDL_Texture* initTexture, 
            Sound* initTurretSpawnSound,
            Sound* initTurretShootSound
        );
    };
};