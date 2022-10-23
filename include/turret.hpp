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
        SDL_Rect* initSourceRect,
        int initDamage,
        float initReload,
        int initRange,
        IPoint initIndex,
        TurretHandler* initTurretHandler
    ) : Sprite {
        initRenderer, 
        initTexture, 
        initDestRect,
        initSourceRect
    },
    damage{initDamage},
    reloadTime{initReload},
    range{initRange},
    index{initIndex}{}

    IPoint Turret::getIndex()
    {
        return index;
    }
    void Turret::currentEnemy() {}
    void Turret::damageEnemy() {}
    Turret::~Turret(){}



    TurretHandler::TurretHandler(
        SDL_Renderer* initRenderer,
        SDL_Rect* initDestRect,
        Map* initMap
        ) : Renderable{initRenderer, initDestRect}, map{initMap}
        {
            readTurretData("turret.txt");
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
        int damage;
        float reloadTime;
        int range;
        SDL_Texture* texture;

        std::getline(turretFile, buffer);
        damage = std::stoi(buffer);
        std::getline(turretFile, buffer);
        reloadTime = std::stof(buffer);
        std::getline(turretFile, buffer);
        range = std::stoi(buffer);
        std::getline(turretFile, buffer);
        SDL_Surface* surface = SDL_LoadBMP(((std::string)"assets/images/" + buffer).c_str());
        texture = SDL_CreateTextureFromSurface(this->renderer, surface);
        SDL_FreeSurface(surface);
        turretTypes.push_back(new TurretData(damage, reloadTime, range, texture));
    }

    void  TurretHandler::createTurret(int type, const IPoint& index)
    {
        TurretData* data = this->turretTypes[type];
        SDL_Rect* rect = this->map->getTileDest(index);
        Turret* turret = new Turret(
            this->renderer,
            data->texture,
            rect,
            nullptr,
            data->damage,
            data->reload,
            data->range,
            index,
            this
        );
        this->turrets.insert(turret);
    }

    void  TurretHandler::handleEvent(SDL_Event* event)
    {
        int x, y;
        if(event->type == SDL_MOUSEBUTTONUP)
        {
            SDL_GetMouseState(&x, &y);
            IPoint index = this->map->getTileIndex({x,y});

            bool placed = this->map->placeTurret(index);

            if(placed)
            {
                createTurret(0, index);
            }
            else if(this->map->getTileType(index)==TileType::TurretType)
            {
                this->map->sellTurret(index);
                std::unordered_set<Turret*>::iterator i = std::find_if(this->turrets.begin(),this->turrets.end(),[&](Turret* turret)->bool{
                    return turret->getIndex()==index;
                });
                delete *i;

                this->turrets.erase(i);

            }
        }
    }

    EnemyHandler TurretHandler::getEnemyHandler()
    {
        return this->
    }

    TurretData::TurretData(
        int initDamage, 
        float initReload, 
        int initRange,
        SDL_Texture* initTexture
    )
    {
        damage = initDamage;
        reload = initReload;
        range = initReload;
        texture = initTexture;
    }
};