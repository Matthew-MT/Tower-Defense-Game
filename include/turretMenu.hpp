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
#include "map_h"

namespace game{
    void TurretMenu::updatePosition(){
        if(this->gatlingRect != nullptr) delete this->gatlingRect;

        IPoint
            position = this->getPosition(),
            size = this->getSize();
        
        this->gatlingRect = fromCenter(
            position + IPoint{
                size.x >> 1,
                size.y >> 1
            },
            IPoint{
                size.x >> 1,
                size.x >> 1
            }
        );
    }

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
            SDL_Surface * surface = IMG_Load("assets/images/turret_menu_select.png");
            this->gatlingSelect = SDL_CreateTextureFromSurface(this->renderer, surface);
            SDL_FreeSurface(surface);
        }

    TurretMenu::~TurretMenu(){
        SDL_DestroyTexture(this->gatling);
        SDL_DestoryTexture(this->gatlingSelect);
    }

    void TurretMenu::render(){
        SDL_RenderCopy(
            this->renderer,
            this->selected == Option::gatling ? this->gatlingSelect : this->gatling,
            nullptr,
            this->gatlingRect
        );
    }

    void MapMenu::handleEvent(SDL_Event* event) {
        if (!this->displayed) return;
        if (event->type == SDL_MOUSEMOTION) {
            IPoint point = {event->motion.x, event->motion.y};
            if (contains(this->gatlingRect, point)) this->selected = Option::gatling;
            else this->hovered = Option::None;
        } else if (event->type == SDL_MOUSEBUTTONUP) {
            if (this->hovered == Option::Left) {
                this->place--;
                while (this->place < 0) this->place = this->mapList.size() + place;
                this->map->loadMap(this->mapList[this->place]);
            } else if (this->hovered == Option::Right) {
                this->place++;
                while (this->place >= this->mapList.size()) this->place -= this->mapList.size();
                this->map->loadMap(this->mapList[this->place]);
            } else if (this->hovered != Option::None) this->map->start(this->hovered);
        }
    }

    void TurretMenu::setDestRect(SDL_Rect* newDestRect) {
        this->Renderable::setDestRect(newDestRect);
        this->updatePosition();
    }

    void TurretMenu::setPosition(const IPoint& position) {
        this->Renderable::setPosition(position);
        this->updatePosition();
    }
    
    void TurretMenu::setPosition(const DPoint& position) {
        this->Renderable::setPosition(position);
        this->updatePosition();
    }

    void TurretMenu::setDisplayed(bool newDisplayed) {
        this->displayed = newDisplayed;
    }
};