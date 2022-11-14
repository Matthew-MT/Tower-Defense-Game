#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "game_state.hpp"
#include "sprite.hpp"
#include "map.hpp"
#include "forward.hpp"
#include "turret_h.hpp"
#include "utils.hpp"
#include <cstring>
#include "sound.hpp"
#include "animation.hpp"
#include <math.h>

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
        double initAngle,
        Sound* initSpawnSound,
        Sound* initShootSound,
        std::string initTurretTexture,
        int initFrames,
        int initMillisPerFrame
    ) : Animation {
        initRenderer, 
        initTexture, 
        initDestRect,
        initSourceRect,
        initAngle,
        initTurretTexture,
        initFrames,
        initMillisPerFrame
    },
    damage{initDamage},
    reloadTime{initReload},
    range{initRange},
    index{initIndex},
    turretHandler{initTurretHandler},
    spawnSound{initSpawnSound},
    shootSound{initShootSound},
    defTexture{texture}{}

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

            if(range>=enemyDistance)
            {
                this->targetedEnemy = enemy;
                this->targetedEnemy->track(this);
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
                    this->texture = getTexture();
                    this->shootSound->playSound();
                    this->remainingReload = reloadTime;
                    
                }
                else
                    this->texture = defTexture;
            }
            else
            {
                stopTracking();
                //this->texture = defTexture;
            }
        }
        else
        {
            this->texture = defTexture;
            this->findTarget();
        }
    }

    void Turret::stopTracking()
    {
        targetedEnemy = nullptr;
    }

    Turret::~Turret(){}

    void Turret::tick(double scalar)
    {
        checkTarget(scalar);
        Animation::tick(scalar);
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
            readTurretData("gatling.txt");
            readTurretData("sniper.txt");
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
        float reloadTime;
        int
            buyPrice,
            sellPrice,
            damage,
            range;
        SDL_Texture
            * texture,
            * menuTexture,
            * menuTextureSelected;

        std::getline(turretFile, buffer);
        buyPrice = std::stoi(buffer);
        std::getline(turretFile, buffer);
        sellPrice = std::stoi(buffer);
        std::getline(turretFile, buffer);
        damage = std::stoi(buffer);
        std::getline(turretFile, buffer);
        reloadTime = std::stof(buffer);
        std::getline(turretFile, buffer);
        range = std::stoi(buffer);
        std::getline(turretFile, buffer);
        SDL_Surface* surface = IMG_Load(((std::string)"assets/images/" + buffer).c_str());
        texture = SDL_CreateTextureFromSurface(this->renderer, surface);
        SDL_FreeSurface(surface);
        std::getline(turretFile, buffer);
        Sound* turretSpawnSound = new Sound("assets/sounds/" + buffer);
        std::getline(turretFile, buffer);
        Sound* turretShootSound = new Sound("assets/sounds/" + buffer);
        std::getline(turretFile, buffer);
        surface = IMG_Load(((std::string)"assets/images/" + buffer).c_str());
        menuTexture = SDL_CreateTextureFromSurface(this->renderer, surface);
        SDL_FreeSurface(surface);
        std::getline(turretFile, buffer);
        surface = IMG_Load(((std::string)"assets/images/" + buffer).c_str());
        menuTextureSelected = SDL_CreateTextureFromSurface(this->renderer, surface);
        SDL_FreeSurface(surface);
        std::getline(turretFile, buffer);
        std::string aFile = buffer;
        std::getline(turretFile, buffer);
        int aFrames = stoi(buffer);
        turretTypes.push_back(new TurretData(
            buyPrice,
            sellPrice,
            damage,
            reloadTime,
            range,
            texture,
            menuTexture,
            menuTextureSelected,
            turretSpawnSound,
            turretShootSound,
            aFile,
            aFrames
        ));
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
            0.0,
            data->turretSpawnSound,
            data->turretShootSound,
            data->animationFile,
            data->animationFrames,
            100
        );
        this->turrets.insert(turret);
    }

    void  TurretHandler::handleEvent(SDL_Event* event)
    {
        Sound* sellSound = new Sound("assets/sounds/coinbag-91016.mp3");
        if(this->started && event->type == SDL_MOUSEBUTTONUP)
        {
            int
                selectedType = this->map->getTurretMenu()->getSelectedType(),
                buyPrice = this->turretTypes.at(selectedType)->buyPrice;
            if (!this->map->getGameState()->buy(buyPrice)) return;
            int x, y;
            SDL_GetMouseState(&x, &y);
            IPoint index = this->map->getTileIndex({x,y});

            bool placed = this->map->placeTurret(index);

            if(placed)
            {
                createTurret(selectedType, index);
            }
            else if(this->map->getTileType(index)==TileType::TurretType)
            {
                sellSound->playSound();
                this->map->getGameState()->earn(buyPrice);
                std::unordered_set<Turret*>::iterator i = std::find_if(this->turrets.begin(),this->turrets.end(),[&](Turret* turret)->bool{
                    return turret->getIndex()==index;
                });
                delete *i;
                this->turrets.erase(i);
                this->map->sellTurret(index);
            } else this->map->getGameState()->earn(buyPrice);
        }
    }

    void TurretHandler::tick(double scalar)
    {
        for(Turret* turret : this->turrets) 
            turret->tick(scalar);
    }

    void TurretHandler::start(Option option) {
        this->started = true;
    }

    EnemyHandler* TurretHandler::getEnemyHandler()
    {
        return this->map->getEnemyHandler();
    }

    Map* TurretHandler::getMap()
    {
        return this->map;
    }

    std::vector<TurretData*> TurretHandler::getTurretTypes() {
        return this->turretTypes;
    }

    TurretData::TurretData(
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
        std::string initAnimationFile,
        int initAnimationFrames
    ) :
        buyPrice{initBuyPrice},
        sellPrice{initSellPrice},
        damage{initDamage},
        reload{initReload},
        range{initRange},
        texture{initTexture},
        menuTexture{initMenuTexture},
        menuTextureSelected{initMenuTextureSelected},
        turretSpawnSound{initTurretSpawnSound},
        turretShootSound{initTurretShootSound},
        animationFile{initAnimationFile},
        animationFrames{initAnimationFrames}{}
};