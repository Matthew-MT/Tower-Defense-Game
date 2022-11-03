#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "turret.hpp"
#include "forward.hpp"
#include "sprite.hpp"
#include "turretMenu_h.hpp"

namespace game{
    TurretMenu::TurretMenu(
        SDL_Renderer* initRenderer,
        SDL_Rect* initDestRect,
        Turret* initTurret,
        std::vector<std::string> initTurretList
    ) 
        : Renderable{
            initRenderer,
            initDestRect
        },
        turret{initTurret} 
        turretList{initTurretList} {
            SDL_Surface * surface = IMG_Load("assets/images/turret_menu.png");
            this->gatling = SDL_CreateTextureFromSurface(this->renderer, surface);
            SDL_FreeSurface(surface);
        }

    TurretMenu::~TurretMenu(){
        SDL_DestroyTexture(this->gatling);
    }

    void TurretMenu::render(){
        SDL_RenderCopy(
            this->renderer,
            this->gatling,
            nullptr,
            this->gatlingRect
        );
    }

}