#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "turret_menu_h.hpp"
#include "forward.hpp"
#include "turret.hpp"
#include "sprite.hpp"
#include "map_h.hpp"
#include <iostream>
#include <fstream>
#include <vector>

namespace game {
    IPoint TurretMenu::getOptionCenterByIndex(int index) {
        int
            spacing = this->destRect->h >> 1,
            leftBegin = (this->destRect->w - (((int)((double)spacing * 1.5) * this->options.size()) - (spacing >> 1))) >> 1;
        return {
            this->destRect->x + leftBegin + ((int)((double)spacing * 1.5) * index),
            this->destRect->y + (this->destRect->h >> 1)
        };
    }

    void TurretMenu::updatePosition() {
        for (int i = 0; i < this->optionRects.size(); i++) {
            if (this->optionRects.at(i) != nullptr) delete this->optionRects.at(i);
            this->optionRects.at(i) = fromCenter(
                this->getOptionCenterByIndex(i),
                this->optionSize
            );
        }
    }

    TurretMenu::TurretMenu(
        SDL_Renderer* initRenderer,
        SDL_Rect* initDestRect,
        const std::vector<TurretData*>& initOptions,
        const IPoint& initOptionSize
    ) :
        Renderable{
            initRenderer,
            initDestRect
        },
        options{initOptions},
        optionSize{initOptionSize} {
        this->optionRects.assign(this->options.size(), nullptr);
        this->updatePosition();
    }

    TurretMenu::~TurretMenu() {}

    void TurretMenu::render(){
        for (int i = 0; i < this->options.size(); i++) {
            SDL_RenderCopy(
                this->renderer,
                i == this->selected ? this->options.at(i)->menuTextureSelected : this->options.at(i)->menuTexture,
                nullptr,
                this->optionRects.at(i)
            );
        }
    }

    void TurretMenu::handleEvent(SDL_Event* event) {
        if (event->type == SDL_MOUSEBUTTONUP) {
            SDL_Log(("Mouse position: (" + std::to_string(event->button.x) + ", " + std::to_string(event->button.y) + ")").c_str());
            for (int i = 0; i < this->optionRects.size(); i++) {
                SDL_Log((
                    "Option rect: ("
                    + std::to_string(this->optionRects.at(i)->x)
                    + ", " + std::to_string(this->optionRects.at(i)->y)
                    + "); (" + std::to_string(this->optionRects.at(i)->w)
                    + ", " + std::to_string(this->optionRects.at(i)->h) + ")"
                ).c_str());
                if (contains(this->optionRects.at(i), {event->button.x, event->button.y})) {
                    this->selected = i;
                    break;
                }
            }
            SDL_Log(("Selected option: " + std::to_string(this->selected)).c_str());
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

    int TurretMenu::getSelectedType() {
        return this->selected;
    }
};