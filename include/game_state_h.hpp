#pragma once
#include <SDL2/SDL.h>
#include "forward.hpp"
#include "sprite.hpp"

namespace game {
    class GameState : public StaticSprite {
    protected:
        int
            maxHealth,
            health,
            cash;
    public:
        GameState(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect,
            int initHealth,
            int initCash
        );
        ~GameState();

        virtual void render();

        virtual void reduceHealth(int amount = 1);
        virtual void increaseHealth(int amount = 1);
        virtual void resetHealth();
        virtual void zeroHealth();
        virtual int getHealth();
        virtual bool isDead();
        virtual bool buy(int amount);
        virtual void earn(int amount);
    };
};
