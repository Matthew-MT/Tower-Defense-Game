#pragma once
#include <SDL2/SDL.h>
#include "forward.hpp"
#include "sprite.hpp"
#include "text.hpp"

namespace game {
    class GameState : public StaticSprite {
    protected:
        Text
            * titleDisplay = nullptr,
            * cashDisplay = nullptr,
            * healthDisplay = nullptr;
        int
            maxHealth,
            cash,
            health;

        IPoint getTitleDisplayPosition();
        IPoint getCashDisplayPositon();
        IPoint getHealthDisplayPosition();

        std::string getCashText();
        std::string getHealthText();

        void updatePosition();
    public:
        GameState(
            SDL_Renderer* initRenderer,
            TTF_Font* initFont,
            SDL_Rect* initDestRect,
            const std::string& title,
            int initCash,
            int initHealth
        );
        ~GameState();

        virtual void render();

        virtual void setDestRect(SDL_Rect* rect);
        virtual void setPosition(const IPoint& position);
        virtual void setPosition(const DPoint& position);

        virtual bool buy(int amount);
        virtual void earn(int amount);
        virtual void reduceHealth(int amount = 1);
        virtual void increaseHealth(int amount = 1);
        virtual void resetHealth();
        virtual void zeroHealth();
        virtual int getHealth();
        virtual bool isDead();
    };
};
