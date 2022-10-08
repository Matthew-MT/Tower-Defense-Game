#pragma once
#include <SDL2/SDL.h>
#include "game_state.hpp"
#include "sprite.hpp"
#include "map.hpp"

namespace game{
    class Turret : public Sprite
    {
        protected:
        GameState* gamestate;
        TurretHandler* handler;
        int damage;
        double reloadTime;
        double angle;
        public:
        Turret(
            SDL_Renderer* initRenderer,
            SDL_Texture* initTexture,
            SDL_Rect* initDestRect,
            SDL_Rect* initSourceRect = nullptr,
            double initAngle = 0.f,
            int initDamage=0,
            double initReload=0.0
        ) : Sprite {
            initRenderer, 
            initTexture, 
            initDestRect,
            initSourceRect,
            initAngle
        },
        angle{initAngle},
        damage{initDamage},
        reloadTime{initReload} {}

        Turret::~Turret() {}
    };

    class TurretHandler : public Renderable
    {
        protected:
        Map* map;
        std::unordered_set<Turret*> turret;
        int damage;
        double reloadTime;
        SDL_Texture* initTexture;
        public:
        TurretHandler(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect
            );
        void readTurretData(const std::string& turretFileName)
        {
            std::fstream 
                turretFile("assets/turrets/" + turretFileName, std::ios_base::in);
            std::string buffer;

            std::getline(turretFile, buffer);
            

        }
        void setTurretData(SDL_Texture* t, int d, float r)
        {
            t = initTexture;
            d = damage;
            r = reloadTime;
        }

    };
}