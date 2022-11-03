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
    class TurretMenu : public renderable{
    protected:
        Turret* turret;
        std::vector<std::string> turretList;
        SDL_Texture
            *gatling;
        SDL_Rect
            *gatlingRect;
    public:
        TurretMenu(
            Turret* initTurret,
            const std::vector<std::string>& initTurretList
        );
        ~TurretMenu();
        virtual void render();
        virtual void handleEvent(SDL_Event* event);
    }
};