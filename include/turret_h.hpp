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
        public:
        Turret(
            SDL_Renderer* initRenderer,
            SDL_Texture* initTexture,
            SDL_Rect* initDestRect,
            SDL_Rect* initSourceRect,
            int initDamage,
            float initReload
        );

        void currentEnemy();
        ~Turret();
    };

    class TurretHandler : public Renderable
    {
        protected:
        Map* map;
        std::unordered_set<Turret*> turrets;
        std::vector<TurretData*>  turretTypes;
        int damage;
        float reloadTime;
        SDL_Texture* texture;
        public:
        TurretHandler(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect
            ); 

        void render();

        void readTurretData(const std::string& turretFileName);

        void storeTurretData(SDL_Texture* t, int d, float r);

        void createTurret(int type);

        void handleEvent(SDL_Event* event);
    };

    class TurretData
    {
        public:
        int damage;
        float reload;
        SDL_Texture* texture;
        public:
        TurretData(
            int initDamage, float initReload, SDL_Texture* initTexture
        );
    };
};