#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "game_state.hpp"
#include "animation.hpp"
#include "forward.hpp"
#include "sprite.hpp"
#include "enums.hpp"
#include "map_h.hpp"
#include <vector>

namespace game{
    class Turret : public Animation {
    protected:
        GameState* gamestate;
        TurretData* data;
        IPoint index;
        TurretHandler* turretHandler;
        Enemy* targetedEnemy = nullptr;
        double remainingReload = 0;
        double angle;
        SDL_Texture* defTexture;
    public:
        Turret(
            SDL_Renderer* initRenderer,
            SDL_Texture* initTexture,
            SDL_Rect* initDestRect,
            SDL_Rect* initSourceRect,
            IPoint initIndex,
            TurretHandler* initTurretHandler,
            TurretData* initData,
            int initMillisPerFrame
        );

        IPoint getIndex();
        int getSellPrice();
        TurretData* getTurretData();

        void checkTarget(double scalar);
        void findTarget();
        void stopTracking();
        void setTurretData(TurretData* data);

        void tick(double scalar);
        void rotateTurret(DPoint enemy, DPoint turret);
        DPoint getCenter();

        ~Turret();
    };

    class TurretHandler : public Renderable {
    protected:
        Map* map;
        std::unordered_set<Turret*> turrets;
        std::vector<TurretData*>  turretTypes;
        TurretUpgradeMenu* turretUpgradeMenu;
        bool started = false;
        Sound* sellSound;
    public:
        TurretHandler(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect,
            Map* initMap,
            TTF_Font* initFont
        );

        void render();

        TurretData* readTurretData(const std::string& turretFileName);
        void createTurret(int type, const IPoint& index);
        bool sellTurret(Turret* turret);
        bool upgradeTurret(Turret* turret);

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
        Sound
            * turretSpawnSound,
            * turretShootSound;
        std::string animationFile;
        int animationFrames;
        void* upgradePath = nullptr;

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
            TurretData* initUpgradePath
        );
    };
};