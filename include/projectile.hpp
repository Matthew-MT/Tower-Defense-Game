#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "enemies_h.hpp"
#include "game_state.hpp"
#include "sprite.hpp"
#include "map_h.hpp"
#include "utils.hpp"
#include <cstring>
#include "sound.hpp"
#include <math.h>
#include "projectile_h.hpp"

namespace game{
    
    Projectile::Projectile(
        SDL_Renderer* initRenderer,
        SDL_Texture* initTexture,
        SDL_Rect* initDestRect,
        SDL_Rect* initSourceRect,
        double initAngle,
        int initProjectileSpeed,
        Enemy* initEnemy,
        int initRange,
        ProjectileHandler* initHandler
    ) : Sprite{
        initRenderer,
        initTexture,
        initDestRect,
        initSourceRect,
        initAngle
    },
    projectileSpeed{initProjectileSpeed},
    targetEnemy{initEnemy},
    range{initRange},
    handler{initHandler}{}

    void Projectile::movingProjectile(double scalar)
    {  
        DPoint enemyPos = targetEnemy->getCenter(); 
        DPoint proj = getCenter();
        rotateProjectile(enemyPos, proj);

        DPoint slope = enemyPos - proj;
        double range = (double)this->projectileSpeed * scalar;

        double normalizeScalar = std::sqrt(std::pow((double)slope.x, 2) + std::pow((double)slope.y, 2)) / range;

        slope.x = slope.x / normalizeScalar;
        slope.y = slope.y / normalizeScalar;

        DPoint newPos = proj+slope;

        this->setCenter(newPos);

    }

    void Projectile::rotateProjectile(DPoint enemy, DPoint proj)
    {
        double eX, eY, pX, pY;
        eX = enemy.x;
        eY = enemy.y;
        pX = proj.x;
        pY = proj.y;

        double fX, fY;
        fX = pX - eX;
        fY = pY - eY;
        double angleRad = atan2(fX,fY);
        if(angleRad < 0) { 
            angleRad = (2 * M_PI) - angleRad; }
        

        const double conversion = 180.0/M_PI;
        double angleDeg = angleRad * conversion;
        if(eX<pX)
        {
            angleDeg = -1 * angleDeg;
        }
        setAngle(angleDeg);
    }

    DPoint Projectile::getCenter()
    {
        return {
            this->position.x + (double)(this->destRect->w >> 1),
            this->position.y + (double)(this->destRect->h >> 1)
        };
    }

    bool Projectile::detectHit(double scalar, DPoint enemyPos)
    {
        DPoint proj = getCenter();
        
        double fDistance = distance(enemyPos, proj);
        SDL_Log(std::to_string(fDistance).c_str());
        double fX,fY;
        fX = abs(enemyPos.x - proj.x);
        fY = abs(enemyPos.y - proj.y);
        SDL_Log(std::to_string((this->projectileSpeed*scalar)+20).c_str());
        if(fDistance <= (this->projectileSpeed*scalar)+20)
            return true;
        else
            return false;
    }

    void Projectile::damageEnemies(int damage)
    {
        for(Enemy* enemy: *this->handler->getEnemyHandler())
        {
            DPoint enemyPos = enemy->getCenter();
            DPoint proj = getCenter();
            double enemyDistance = distance(proj, enemyPos);
            if(range>=enemyDistance)
            {
                enemy->damage(damage);
            }
        }
    }

    void Projectile::deleteProj(Projectile* p)
    {
        delete p;
    }
    void Projectile::tick(double scalar)
    {
        movingProjectile(scalar);
    }

    ProjectileHandler::ProjectileHandler(
        SDL_Renderer* initRenderer,
        SDL_Rect* initDestRect,
        Map* initMap
    ) : Renderable{initRenderer, initDestRect}, map{initMap}{}

    void ProjectileHandler::render()
    {
        for(Projectile* p: this->projectiles)
        {
            p->render();
        }
    }

    Projectile* ProjectileHandler::createProjectile(SDL_Rect* dRect, double ang, Enemy* enemy)
    {
        std::string file = "assets/images/projectiles/missle1.bmp";
        SDL_Surface* surface = SDL_LoadBMP(file.c_str());
        SDL_Texture* Texture = SDL_CreateTextureFromSurface(this->renderer, surface);
        SDL_FreeSurface(surface);
        SDL_Rect* rect = new SDL_Rect();
        rect->x = dRect->x;
        rect->y = dRect->y;
        rect->w = dRect->w;
        rect->h = dRect->h;


           Projectile* p = new Projectile(
            this->renderer,
            Texture,
            rect,
            nullptr,
            ang,
            100,
            enemy,
            80,
            this);

        this->projectiles.insert(p);
        
        return p;
    }

    void ProjectileHandler::deleteProjectile(Projectile* p)
    {
        p->deleteProj(p);
        this->projectiles.erase(p);
    }

    void ProjectileHandler::tick(double scalar)
    {
        for(Projectile* p: this->projectiles)
        {
            p->tick(scalar);
        }   
    }

    EnemyHandler* ProjectileHandler::getEnemyHandler()
    {
        return this->map->getEnemyHandler();
    }

}