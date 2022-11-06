#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "turret.hpp"
#include "forward.hpp"
#include "sprite.hpp"

namespace game{
    class TurretMenu : public Renderable {
    protected:
        Turret* turret;
        std::vector<std::string> turretList;
        SDL_Texture
            *gatling, 
            *gatlingSelect;
        SDL_Rect
            *gatlingRect = nullptr;
        Option selected = Option::None;

        virtual void updatePosition();
    public:
        TurretMenu(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect,
            Turret* initTurret
        );
        ~TurretMenu();
        virtual void render();
        virtual void handleEvent(SDL_Event* event);

        virtual void setDestRect(SDL_Rect* newDestRect);
        virtual void setPosition(const IPoint& position);
        virtual void setPosition(const DPoint& position);
        virtual void setDisplayed(bool newDisplayed);
    }
};