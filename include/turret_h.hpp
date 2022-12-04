#pragma once
#include <SDL2/SDL.h>
#include "game_state.hpp"
#include "forward.hpp"
#include "sprite.hpp"
#include "enums.hpp"
#include "map.hpp"
#include <vector>
#include "animation.hpp"

namespace game{
    class Turret : public Animation
    {
        protected:
        GameState* gamestate;
        int damage;
        double reloadTime;
        int range;
        IPoint index;
        TurretHandler* turretHandler;
        Enemy* targetedEnemy = nullptr;
        std::vector<Enemy*> targetedEnemies;
        double remainingReload=0;
        double angle;
        Sound* spawnSound;
        SDL_Texture* defTexture;
        Sound* shootSound;
        int turretKind;
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
            Sound* initShootSound,
            std::string initTurretTexture,
            int initFrames,
            int initMillisPerFrame,
            int initTurretKind
        );

        IPoint getIndex();

        
        void checkTarget(double scalar);
        void findTarget(int i);
        void stopTracking();
        bool searchTargets(Enemy* enemy);

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

        std::vector<TurretData*> getTurretTypes();
    };

    class TurretData
    {
        public:
        double reload;
        int
            buyPrice,
            sellPrice,
            damage,
            range;
        SDL_Texture
            * texture,
            * menuTexture,
            * menuTextureSelected;
        Sound* turretSpawnSound;
        Sound* turretShootSound;
        std::string animationFile;
        int animationFrames;
        int turretKind;
        public:
        TurretData(
            int initBuyPrice,
            int initSellPrice,
            int initDamage, 
            double initReload, 
            int initRange,
            SDL_Texture* initTexture,
            SDL_Texture* initMenuTexture,
            SDL_Texture* initMenuTextureSelected,
            Sound* initTurretSpawnSound,
            Sound* initTurretShootSound,
            std::string animationFile,
            int initAnimationFrames,
            int initTurretKind
        );
    };
};