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
        float reloadTime;
        public:
        Turret(
            SDL_Renderer* initRenderer,
            SDL_Texture* initTexture,
            SDL_Rect* initDestRect,
            SDL_Rect* initSourceRect = nullptr,
            int initDamage=0,
            double initReload=0.0
        ) : Sprite {
            initRenderer, 
            initTexture, 
            initDestRect,
            initSourceRect
        },
        damage{initDamage},
        reloadTime{initReload} {}

        void currentEnemy(){

        }
        ~Turret() {}
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
            ) : Renderable{initRenderer, initDestRect}
            {
                readTurretData("turret.txt");
                storeTurretData(texture, damage, reloadTime);
            }

        void render()
        {
            for(Turret* turret: this->turrets)
            {
                turret->render();
            }
        }    

        void readTurretData(const std::string& turretFileName)
        {
            std::fstream 
                turretFile("assets/turrets/" + turretFileName, std::ios_base::in);
            std::string buffer;

            std::getline(turretFile, buffer);
            damage = std::stoi(buffer);
            std::getline(turretFile, buffer);
            reloadTime = std::stof(buffer);
            std::getline(turretFile, buffer);
            SDL_Surface* surface = SDL_LoadBMP(((std::string)"assets/images/" + buffer).c_str());
            texture = SDL_CreateTextureFromSurface(this->renderer, surface);
            SDL_FreeSurface(surface);
        }

        void storeTurretData(SDL_Texture* t, int d, float r)
        {
            turretTypes.push_back(new TurretData(d, r, t));
        }

        void createTurret(TurretType type)
        {
            TurretData* data = this->turretTypes[type];
            this->turrets.insert(new Turret(
                this->renderer,
                data->texture,
                this->destRect,
                nullptr,
                data->damage,
                data->reload
            ));
        }

        void handleEvent(SDL_Event* event)
        {
            int x, y;
            if(event->type == SDL_MOUSEBUTTONUP)
            {
                SDL_GetGlobalMouseState(&x, &y);
                IPoint index = this->map->getTileIndex({x,y});
                bool placed = this->map->placeTurret(index);
                if(placed)
                {
                    createTurret(Gatling);
                }
            }
        }
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
        )
        {
            damage = initDamage;
            reload = initReload;
            texture = initTexture;
        }
    };
}