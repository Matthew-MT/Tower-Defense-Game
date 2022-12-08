#pragma once
#include <SDL2/SDL.h>
#include "game_state.hpp"
#include "sprite.hpp"
#include "enums.hpp"
#include <vector>

namespace game{
    class Projectile : public Sprite
    {
        protected:
        int projectileSpeed;
        Enemy* targetEnemy = nullptr;
        int range;
        ProjectileHandler* handler;

        public:

        Projectile(
            SDL_Renderer* initRenderer,
            SDL_Texture* initTexture,
            SDL_Rect* initDestRect,
            SDL_Rect* initSourceRect,
            double initAngle,
            int initProjectileSpeed,
            Enemy* initEnemy,
            int initRange,
            ProjectileHandler* initHandler
        );

        void rotateProjectile(DPoint enemy, DPoint proj);
        DPoint getCenter();
        void movingProjectile(double scalar);
        bool detectHit(double scalar, DPoint enemy);
        void deleteProj(Projectile* p);
        void tick(double scalar);
        void damageEnemies(int damage);
    };

    class ProjectileHandler : public Renderable
    {
        protected:
        Map* map;
        std::unordered_set<Projectile*> projectiles;

        public:
        ProjectileHandler(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect,
            Map* initMap
        );

        void render();
        Projectile* createProjectile(SDL_Rect* dRect, double ang, Enemy* enemy);
        void deleteProjectile(Projectile* p);
        void tick(double scalar);
        EnemyHandler* getEnemyHandler();
    };
}