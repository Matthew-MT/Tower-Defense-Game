#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "turret_upgrade_menu.hpp"
#include "game_state.hpp"
#include "animation.hpp"
#include "turret_h.hpp"
#include "forward.hpp"
#include "sprite.hpp"
#include "sound.hpp"
#include "utils.hpp"
#include <cstring>
#include "sound.hpp"
#include "animation.hpp"
#include "projectile.hpp"
#include <math.h>
#include <vector>

namespace game {
    Turret::Turret(
        SDL_Renderer* initRenderer,
        SDL_Texture* initTexture,
        SDL_Rect* initDestRect,
        SDL_Rect* initSourceRect,
        IPoint initIndex,
        TurretHandler* initTurretHandler,
        TurretData* initData,
        int initMillisPerFrame
    ) :
        Animation {
            initRenderer, 
            initTexture, 
            initDestRect,
            initSourceRect,
            0.0,
            initData->animationFile,
            initData->animationFrames,
            initMillisPerFrame
        },
        index{initIndex},
        turretHandler{initTurretHandler},
        defTexture{texture},
        data{initData} {}

    IPoint Turret::getIndex() {
        return index;
    }

    int Turret::getSellPrice() {
        return ((TurretData*)this->data)->sellPrice;
    }

    TurretData* Turret::getTurretData() {
        return ((TurretData*)this->data);
    }

    void Turret::findTarget() {
        for(Enemy* enemy : *this->turretHandler->getEnemyHandler()) {
            DPoint enemyPosition = enemy->getCenter();
            DPoint turretPosition = (DPoint)this->turretHandler->getMap()->getTileCenter(this->index);
            double enemyDistance = distance(turretPosition, enemyPosition);

            if (((TurretData*)this->data)->range >= enemyDistance) {
                this->targetedEnemy = enemy;
                this->targetedEnemy->track(this);
                break;
            }
        }
        return;
    }

    void Turret::damageEnemies(int damage)
    {
        for(Enemy* enemy: *this->turretHandler->getEnemyHandler())
        {
            DPoint enemyPos = enemy->getCenter();
            DPoint proj = getCenter();
            double enemyDistance = distance(proj, enemyPos);
            if(((TurretData*)this->data)->range>=enemyDistance)
            {
                enemy->damage(damage);
            }
        }
    }

    void Turret::checkTarget(double scalar) {
        if (remainingReload>0) {
            this->remainingReload-=scalar;
        }
        
        if(this->targetedEnemy != nullptr || this->targetedEnemies.empty() != true)
        {
            DPoint enemyPosition = targetedEnemy->getCenter();
            DPoint turretPosition = (DPoint)this->turretHandler->getMap()->getTileCenter(this->index);
            double enemyDistance = distance(turretPosition,enemyPosition);
            if (((TurretData*)this->data)->range >= enemyDistance) 
            {
                this->rotateTurret(this->targetedEnemy->getCenter(), this->getCenter());
                if( ((TurretData*)this->data)->tType == 0)
                {
                    if(remainingReload<=0)
                    {
                        this->targetedEnemy->damage(((TurretData*)this->data)->damage);
                        this->texture = getTexture();
                        ((TurretData*)this->data)->turretShootSound->playSound();
                        this->remainingReload = ((TurretData*)this->data)->reload;
                        
                    }
                    else
                        this->texture = defTexture;
                    
                }
                else if (((TurretData*)this->data)->tType == 1)
                {
                    if(remainingReload<=0 && proj == nullptr)
                    {
                        this->texture = getTexture();
                        ((TurretData*)this->data)->turretShootSound->playSound();
                        this->remainingReload = ((TurretData*)this->data)->reload;
                        this->proj = turretHandler->createProjectile(this->destRect, this->angle, this->targetedEnemy);
                    }
                    else
                        this->texture = defTexture;    
                }
                if(((TurretData*)this->data)->tType == 2)
                {
                    if(remainingReload<=0)
                    {
                        damageEnemies(((TurretData*)this->data)->damage);
                        this->texture = getTexture();
                        ((TurretData*)this->data)->turretShootSound->playSound();
                        this->remainingReload = ((TurretData*)this->data)->reload;
                        this->texture = defTexture;
                    }
                }
                
            }
            else
            {
                stopTracking();
                //this->texture = defTexture;
            }
           
            if(this->proj != nullptr && this->proj->detectHit(scalar, enemyPosition) == true)
            {   
                this->proj->damageEnemies(((TurretData*)this->data)->damage);
                turretHandler->deleteProjectile(this->proj);
                //this->targetedEnemy->damage(this->damage);
                proj = nullptr;
            }
        }
        else
        {
            if(this->targetedEnemy == nullptr && this->proj != nullptr)
            {
                turretHandler->deleteProjectile(this->proj);
                proj = nullptr;
            }
            this->texture = defTexture;
            this->findTarget();
        }
    }


    void Turret::stopTracking()
    {
        this->targetedEnemy = nullptr;
        targetedEnemies.clear();
    }

    bool Turret::searchTargets(Enemy* enemy){
        for (int i = 0; i < this->targetedEnemies.size(); i++){
            if (this->targetedEnemies[i] == enemy) return true;
        }
        return false;
    }

    void Turret::setTurretData(TurretData* data) {
        this->data = data;
    }

    Turret::~Turret() {}

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
        double angleRad = atan2(fX, fY);
        if (angleRad < 0) { 
            angleRad = (2 * M_PI) - angleRad;
        }

        const double conversion = 180.0 / M_PI;
        double angleDeg = angleRad * conversion;
        if (eX < tX) {
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
        Map* initMap,
        TTF_Font* initFont
    ) :
        Renderable{initRenderer, initDestRect},
        map{initMap},
        projectileHandler{new ProjectileHandler(
        this->renderer,
        this->destRect,
        this->getMap())},
        turretUpgradeMenu{new TurretUpgradeMenu(
            initRenderer,
            initMap,
            this,
            initFont
        )},
        sellSound{new Sound("assets/sounds/coinbag-91016.mp3")} {
        this->turretTypes.push_back(readTurretData("gatling.txt"));
        this->turretTypes.push_back(readTurretData("sniper.txt"));
        this->turretTypes.push_back(readTurretData("rocket.txt"));
        this->turretTypes.push_back(readTurretData("tesla.txt"));
    }

    void TurretHandler::render() {
        for(Turret* turret: this->turrets) {
            turret->render();
        }
        projectileHandler->render();
        this->turretUpgradeMenu->render();
    }    

    TurretData* TurretHandler::readTurretData(const std::string& turretFileName) {
        std::fstream 
            turretFile("assets/turrets/" + turretFileName, std::ios_base::in);
        std::string buffer;
        float reloadTime;
        int
            buyPrice,
            sellPrice,
            damage,
            range,
            tType;
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
        std::getline(turretFile, buffer);
        tType = stoi(buffer);

        TurretData* upgradePath = nullptr;
        if (!turretFile.eof()) {
            std::getline(turretFile, buffer);
            upgradePath = this->readTurretData(buffer);
        }

        return new TurretData(
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
            aFrames,
            tType,
            upgradePath
        );
    }

    void  TurretHandler::createTurret(int type, const IPoint& index) {
        TurretData* data = this->turretTypes[type];
        data->turretSpawnSound->playSound();
        SDL_Rect* rect = this->map->getTileDest(index);
        Turret* turret = new Turret(
            this->renderer,
            data->texture,
            rect,
            nullptr,
            index,
            this,
            data,
            100
        );
        
        this->turrets.insert(turret);
    }

    bool TurretHandler::sellTurret(Turret* turret) {
        IPoint index = turret->getIndex();
        this->sellSound->playSound();
        std::unordered_set<Turret*>::iterator i = std::find_if(
            this->turrets.begin(),
            this->turrets.end(),
            [&](Turret* turret) -> bool {
                return turret->getIndex() == index;
            }
        );
        if (i != this->turrets.end()) {
            this->map->getGameState()->earn(turret->getSellPrice());
            this->turrets.erase(i);
            delete turret;
            this->map->sellTurret(index);
            return true;
        } else return false;
    }

    bool TurretHandler::upgradeTurret(Turret* turret) {
        IPoint index = turret->getIndex();
        std::unordered_set<Turret*>::iterator i = std::find_if(
            this->turrets.begin(),
            this->turrets.end(),
            [&](Turret* turret) -> bool {
                return turret->getIndex() == index;
            }
        );
        if (
            i != this->turrets.end()
            && turret->getTurretData()->upgradePath != nullptr
            && this->map->getGameState()->canBuy(((TurretData*)turret->getTurretData()->upgradePath)->buyPrice)
        ) {
            this->map->getGameState()->buy(((TurretData*)turret->getTurretData()->upgradePath)->buyPrice);
            turret->setTurretData(((TurretData*)turret->getTurretData()->upgradePath));
            return true;
        } else return false;
    }

    void  TurretHandler::handleEvent(SDL_Event* event) {
        if (this->turretUpgradeMenu->isShown()) this->turretUpgradeMenu->handleEvent(event);
        else {
            if (this->started && event->type == SDL_MOUSEBUTTONUP) {
                IPoint index = this->map->getTileIndex({event->button.x, event->button.y});
                int
                    selectedType = this->map->getTurretTypeMenu()->getSelectedType(),
                    buyPrice = this->turretTypes.at(selectedType)->buyPrice,
                    tileType = this->map->getTileType(index);

                if (tileType == TurretType) {
                    std::unordered_set<Turret*>::iterator i = std::find_if(
                        this->turrets.begin(),
                        this->turrets.end(),
                        [&](Turret* turret) -> bool {
                            return turret->getIndex() == index;
                        }
                    );
                    if (i != this->turrets.end()) this->turretUpgradeMenu->loadMenuFor(*i, index);
                } else if (this->map->getGameState()->canBuy(buyPrice) && this->map->placeTurret(index)) {
                    if (!this->map->getGameState()->buy(buyPrice)) {
                        this->map->sellTurret(index);
                        return;
                    }
                    this->createTurret(selectedType, index);
                }
            }
        }
    }

    void TurretHandler::tick(double scalar) {
        for(Turret* turret : this->turrets) 
            turret->tick(scalar);
         
        projectileHandler->tick(scalar);
    
    }

    void TurretHandler::start(Option option) {
        this->started = true;
    }

    EnemyHandler* TurretHandler::getEnemyHandler() {
        return this->map->getEnemyHandler();
    }

    Map* TurretHandler::getMap() {
        return this->map;
    }

    std::vector<TurretData*> TurretHandler::getTurretTypes() {
        return this->turretTypes;
    }

    Projectile* TurretHandler::createProjectile(SDL_Rect* dRect, double ang, Enemy* enemy)
    {
        return projectileHandler->createProjectile(dRect, ang, enemy);
    }

    void TurretHandler::deleteProjectile(Projectile* p)
    {
        projectileHandler->deleteProjectile(p);
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
        int initAnimationFrames,
        int initTurrType,
        TurretData* initUpgradePath=nullptr
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
        animationFrames{initAnimationFrames},
        tType{initTurrType},
        upgradePath{initUpgradePath}{}
        
};