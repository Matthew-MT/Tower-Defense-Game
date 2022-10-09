#pragma once
#include <SDL2/SDL.h>
#include "game_state.hpp"
#include "sprite.hpp"
#include "map.hpp"
#include "forward.hpp"
#include "turret_h.hpp"

namespace game{
    Turret::Turret(
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
    reloadTime{initReload}{}

    void Turret::currentEnemy(){

    }
    Turret::~Turret(){}



    TurretHandler::TurretHandler(
        SDL_Renderer* initRenderer,
        SDL_Rect* initDestRect
        ) : Renderable{initRenderer, initDestRect}
        {
            readTurretData("turret.txt");
            storeTurretData(texture, damage, reloadTime);
        }

    void TurretHandler::render()
    {
        for(Turret* turret: this->turrets)
        {
            turret->render();
        }
    }    

    void  TurretHandler::readTurretData(const std::string& turretFileName)
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

    void  TurretHandler::storeTurretData(SDL_Texture* t, int d, float r)
    {
        turretTypes.push_back(new TurretData(d, r, t));
    }

    void  TurretHandler::createTurret(TurretType type)
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

    void  TurretHandler::handleEvent(SDL_Event* event)
    {
        int x, y;
        SDL_Log("Handling event");
        if(event->type == SDL_MOUSEBUTTONUP)
        {
            SDL_GetGlobalMouseState(&x, &y);
            IPoint index = this->map->getTileIndex({x,y});
            std::cout << index.x << "   " << index.y << std::endl;
            bool placed = this->map->placeTurret(index);
            if(placed)
            {
                SDL_Log("Turret was placed");
                createTurret(Gatling);
            }
        }
    }

    TurretData::TurretData(
        int initDamage, 
        float initReload, 
        SDL_Texture* initTexture
    )
    {
        damage = initDamage;
        reload = initReload;
        texture = initTexture;
    }
};