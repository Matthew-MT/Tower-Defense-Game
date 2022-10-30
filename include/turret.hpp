#pragma once
#include <SDL2/SDL.h>
#include "game_state.hpp"
#include "sprite.hpp"
#include "map.hpp"
#include "forward.hpp"
#include "turret_h.hpp"
#include "utils.hpp"
#include <cstring>

#include "sound.hpp"

namespace game{
    Turret::Turret(
        SDL_Renderer* initRenderer,
        SDL_Texture* initTexture,
        SDL_Rect* initDestRect,
        SDL_Rect* initSourceRect,
        int initDamage,
        double initReload,
        int initRange,
        IPoint initIndex,
        TurretHandler* initTurretHandler,
        double initAngle
    ) : Sprite {
        initRenderer, 
        initTexture, 
        initDestRect,
        initSourceRect,
        initAngle
    },
    damage{initDamage},
    reloadTime{initReload},
    range{initRange},
    index{initIndex},
    turretHandler{initTurretHandler}
    {}

    IPoint Turret::getIndex()
    {
        return index;
    }

    void Turret::findTarget()
    {
        for(Enemy* enemy : *this->turretHandler->getEnemyHandler())
        {
            DPoint enemyPosition = enemy->getCenter();
            DPoint turretPosition = (DPoint)this->turretHandler->getMap()->getTileCenter(this->index);
            double enemyDistance = distance(turretPosition,enemyPosition);

            SDL_Log(std::to_string(range).c_str());

            if(range>=enemyDistance)
            {
                this->targetedEnemy = enemy;
                //rotateTurret(enemyPosition, turretPosition);
                SDL_Log("Enemy targeted");
                break;
            }
        }
    }

    void Turret::checkTarget(double scalar) {
        if(remainingReload>0)
        {
            this->remainingReload-=scalar;
        }

        
        if(this->targetedEnemy != nullptr)
        {
            DPoint enemyPosition = targetedEnemy->getCenter();
            DPoint turretPosition = (DPoint)this->turretHandler->getMap()->getTileCenter(this->index);
            double enemyDistance = distance(turretPosition,enemyPosition);
            if(range>=enemyDistance)
            {
                this->rotateTurret(this->targetedEnemy->getCenter(), this->getCenter());
                if(remainingReload<=0)
                {
                    this->targetedEnemy->damage(this->damage);
                    SDL_Log("Enemy damaged");
                    this->remainingReload = reloadTime;
                }
            }
            else{
                targetedEnemy = nullptr;
            }
        }
        else   
            this->findTarget();
    }
    Turret::~Turret(){}

    void Turret::tick(double scalar)
    {
        checkTarget(scalar);
    }

    void Turret::rotateTurret(DPoint enemy, DPoint turret)
    {
        double eX, eY, tX, tY;
        eX = enemy.x;
        eY = enemy.y;
        tX = turret.x;
        tY = turret.y;

        double fX, fY;
        fX = tX - eX;
        fY = tY - eY;
        double angleRad = atan2(fX,fY);
        if(angleRad < 0) { 
            angleRad = (2 * M_PI) - angleRad; }
        

        const double conversion = 180.0/M_PI;
        double angleDeg = angleRad * conversion;
        if(eX<tX)
        {
            angleDeg = -1 * angleDeg;
        }
        setAngle(angleDeg);
    }

    DPoint Turret::getCenter()
    {
        return {
            this->position.x + (double)(this->destRect->w >> 1),
            this->position.y + (double)(this->destRect->h >> 1)
        };
    }


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

        SDL_Log((buffer).c_str());

        std::getline(turretFile, buffer);
        SDL_Surface* surface = SDL_LoadBMP(((std::string)"assets/images/" + buffer).c_str());
        texture = SDL_CreateTextureFromSurface(this->renderer, surface);
        SDL_FreeSurface(surface);
<<<<<<< HEAD
        turretTypes.push_back(new TurretData(damage, reloadTime, range, texture));
=======
        std::getline(turretFile, buffer);
        Sound* turretSpawnSound = new Sound("assets/sound/" + buffer);
        turretTypes.push_back(new TurretData(damage, reloadTime, texture, turretSpawnSound));
>>>>>>> 0888c397cec66101ca2d2824f0b8783378373465
    }

    void  TurretHandler::createTurret(int type, const IPoint& index)
    {
        TurretData* data = this->turretTypes[type];
        data->turretSpawnSound->playSound();
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
            this,
            0.0
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

    void TurretHandler::tick(double scalar)
    {
        for(Turret* turret : this->turrets) 
            turret->tick(scalar);
    }

    EnemyHandler* TurretHandler::getEnemyHandler()
    {
        return this->map->getEnemyHandler();
    }

    Map* TurretHandler::getMap()
    {
        return this->map;
    }


    TurretData::TurretData(
        int initDamage, 
<<<<<<< HEAD
        double initReload, 
        int initRange,
        SDL_Texture* initTexture
=======
        float initReload, 
        SDL_Texture* initTexture,
        Sound* initTurretSpawnSound
>>>>>>> 0888c397cec66101ca2d2824f0b8783378373465
    )
    {
        damage = initDamage;
        reload = initReload;
        range = initRange;
        texture = initTexture;
        turretSpawnSound = initTurretSpawnSound;
    }
};