#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "turret_type_menu_h.hpp"
#include "turret_h.hpp"
#include "forward.hpp"
#include "sprite.hpp"
#include "map_h.hpp"
#include <iostream>
#include <fstream>
#include <vector>

namespace game {
    IPoint TurretTypeMenu::getOptionCenterByIndex(int index) {
        int
            spacing = this->destRect->h >> 1,
            leftBegin = (this->destRect->w - ((int)((double)spacing * 1.5) * (this->options.size() - 1))) >> 1;
        return {
            this->destRect->x + leftBegin + ((int)((double)spacing * 1.5) * index),
            this->destRect->y + (this->destRect->h >> 1)
        };
    }

    void TurretTypeMenu::updatePosition() {
        for (int i = 0; i < this->optionRects.size(); i++) {
            if (this->optionRects.at(i) != nullptr) delete this->optionRects.at(i);
            this->optionRects.at(i) = fromCenter(
                this->getOptionCenterByIndex(i),
                this->optionSize
            );
        }
    }

    TurretTypeMenu::TurretTypeMenu(
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

    TurretTypeMenu::~TurretTypeMenu() {}

    void TurretTypeMenu::render(){
        for (int i = 0; i < this->options.size(); i++) {
            SDL_RenderCopy(
                this->renderer,
                i == this->selected ? this->options.at(i)->menuTextureSelected : this->options.at(i)->menuTexture,
                nullptr,
                this->optionRects.at(i)
            );
        }
    }

    void TurretTypeMenu::handleEvent(SDL_Event* event) {
        if (event->type == SDL_MOUSEBUTTONUP) {
            for (int i = 0; i < this->optionRects.size(); i++) {
                if (contains(this->optionRects.at(i), {event->button.x, event->button.y})) {
                    this->selected = i;
                    break;
                }
            }
        }
    }

    void TurretTypeMenu::setDestRect(SDL_Rect* newDestRect) {
        this->Renderable::setDestRect(newDestRect);
        this->updatePosition();
    }

    void TurretTypeMenu::setPosition(const IPoint& position) {
        this->Renderable::setPosition(position);
        this->updatePosition();
    }
    
    void TurretTypeMenu::setPosition(const DPoint& position) {
        this->Renderable::setPosition(position);
        this->updatePosition();
    }

    int TurretTypeMenu::getSelectedType() {
        return this->selected;
    }
};