#pragma once
#include <SDL2/SDL.h>
#include "game_state.hpp"
#include "sprite.hpp"
#include "map.hpp"
#include "forward.hpp"

namespace game{
    class Turret : public Sprite
    {
        protected:
        GameState* gamestate;
        int damage;
        float reloadTime;
        int range;
        IPoint index;
        public:
        Turret(
            SDL_Renderer* initRenderer,
            SDL_Texture* initTexture,
            SDL_Rect* initDestRect,
            SDL_Rect* initSourceRect,
            int initDamage,
            float initReload,
            int initRange,
            IPoint initIndex
        );

        IPoint getIndex();
        void currentEnemy();
        ~Turret();
    };

    class TurretHandler : public Renderable
    {
        protected:
        Map* map;
        std::unordered_set<Turret*> turrets;
        std::vector<TurretData*>  turretTypes;
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
    };

    class TurretData
    {
        public:
        int damage;
        float reload;
        int range;
        SDL_Texture* texture;
        public:
        TurretData(
            int initDamage, float initReload, int initRange, SDL_Texture* initTexture
        );
    };
};