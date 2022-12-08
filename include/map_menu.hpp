#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "map_menu_h.hpp"
#include "forward.hpp"
#include "sprite.hpp"
#include "map_h.hpp"
#include <vector>

namespace game {
    void MapMenu::updatePosition() {
        if (this->arrowRightRect != nullptr) delete this->arrowRightRect;
        if (this->arrowLeftRect != nullptr) delete this->arrowLeftRect;
        if (this->difficultyEasyRect != nullptr) delete this->difficultyEasyRect;
        if (this->difficultyNormalRect != nullptr) delete this->difficultyNormalRect;
        if (this->difficultyHardRect != nullptr) delete this->difficultyHardRect;
        if (this->difficultyFunRect != nullptr) delete this->difficultyFunRect;

        IPoint
            position = this->getPosition(),
            size = this->getSize();

        this->arrowRightRect = fromCenter(
            position + IPoint{
                (size.x >> 2) + (size.x >> 1),
                size.y >> 1
            },
            IPoint{
                size.x >> 2,
                size.x >> 2
            }
        );

        this->arrowLeftRect = fromCenter(
            position + IPoint{
                size.x >> 2,
                size.y >> 1
            },
            IPoint{
                size.x >> 2,
                size.x >> 2
            }
        );

        this->difficultyEasyRect = fromCenter(
            position + IPoint{
                size.x >> 1,
                (size.y >> 2) + (size.y >> 4)
            },
            IPoint{
                size.x >> 2,
                (size.x >> 4) + (size.x >> 5)
            }
        );
        
        this->difficultyNormalRect = fromCenter(
            position + IPoint{
                size.x >> 1,
                (size.y >> 1) - (size.y >> 4)
            },
            IPoint{
                size.x >> 2,
                (size.x >> 4) + (size.x >> 5)
            }
        );

        this->difficultyHardRect = fromCenter(
            position + IPoint{
                size.x >> 1,
                (size.y >> 1) + (size.y >> 4)
            },
            IPoint{
                size.x >> 2,
                (size.x >> 4) + (size.x >> 5)
            }
        );

        this->difficultyFunRect = fromCenter(
            position + IPoint{
                size.x >> 1,
                ((size.y >> 1) + (size.y >> 2)) - (size.y >> 4)
            },
            IPoint{
                size.x >> 2,
                (size.x >> 4) + (size.x >> 5)
            }
        );
    }

    MapMenu::MapMenu(
        SDL_Renderer* initRenderer,
        SDL_Rect* initDestRect,
        Map* initMap,
        const std::vector<std::string>& initMapList
    ) :
        Renderable{
            initRenderer,
            initDestRect
        },
        map{initMap},
        mapList{initMapList} {
        SDL_Surface* surface = IMG_Load("assets/images/map_menu/arrow_right.png");
        this->arrow = SDL_CreateTextureFromSurface(this->renderer, surface);
        SDL_FreeSurface(surface);
        surface = IMG_Load("assets/images/map_menu/arrow_right_hovered.png");
        this->arrowHovered = SDL_CreateTextureFromSurface(this->renderer, surface);
        SDL_FreeSurface(surface);
        surface = IMG_Load("assets/images/map_menu/difficulty_easy.png");
        this->difficultyEasy = SDL_CreateTextureFromSurface(this->renderer, surface);
        SDL_FreeSurface(surface);
        surface = IMG_Load("assets/images/map_menu/difficulty_easy_hovered.png");
        this->difficultyEasyHovered = SDL_CreateTextureFromSurface(this->renderer, surface);
        SDL_FreeSurface(surface);
        surface = IMG_Load("assets/images/map_menu/difficulty_normal.png");
        this->difficultyNormal = SDL_CreateTextureFromSurface(this->renderer, surface);
        SDL_FreeSurface(surface);
        surface = IMG_Load("assets/images/map_menu/difficulty_normal_hovered.png");
        this->difficultyNormalHovered = SDL_CreateTextureFromSurface(this->renderer, surface);
        SDL_FreeSurface(surface);
        surface = IMG_Load("assets/images/map_menu/difficulty_hard.png");
        this->difficultyHard = SDL_CreateTextureFromSurface(this->renderer, surface);
        SDL_FreeSurface(surface);
        surface = IMG_Load("assets/images/map_menu/difficulty_hard_hovered.png");
        this->difficultyHardHovered = SDL_CreateTextureFromSurface(this->renderer, surface);
        SDL_FreeSurface(surface);
        surface = IMG_Load("assets/images/map_menu/difficulty_fun.png");
        this->difficultyFun = SDL_CreateTextureFromSurface(this->renderer, surface);
        SDL_FreeSurface(surface);
        surface = IMG_Load("assets/images/map_menu/difficulty_fun_hovered.png");
        this->difficultyFunHovered = SDL_CreateTextureFromSurface(this->renderer, surface);
        SDL_FreeSurface(surface);
        this->map->loadMap(this->mapList.front());
    }

    MapMenu::~MapMenu() {
        SDL_DestroyTexture(this->arrow);
        SDL_DestroyTexture(this->arrowHovered);
        SDL_DestroyTexture(this->difficultyEasy);
        SDL_DestroyTexture(this->difficultyEasyHovered);
        SDL_DestroyTexture(this->difficultyNormal);
        SDL_DestroyTexture(this->difficultyNormalHovered);
        SDL_DestroyTexture(this->difficultyHard);
        SDL_DestroyTexture(this->difficultyHardHovered);
        SDL_DestroyTexture(this->difficultyFun);
        SDL_DestroyTexture(this->difficultyFunHovered);
    }

    void MapMenu::render() {
        if (!this->displayed) return;
        SDL_RenderCopy(
            this->renderer,
            this->hovered == Right ? this->arrowHovered : this->arrow,
            nullptr,
            this->arrowRightRect
        );
        SDL_RenderCopyEx(
            this->renderer,
            this->hovered == Left ? this->arrowHovered : this->arrow,
            nullptr,
            this->arrowLeftRect,
            0.f, nullptr,
            SDL_FLIP_HORIZONTAL
        );
        SDL_RenderCopy(
            this->renderer,
            this->hovered == Easy ? this->difficultyEasyHovered : this->difficultyEasy,
            nullptr,
            this->difficultyEasyRect
        );
        SDL_RenderCopy(
            this->renderer,
            this->hovered == Normal ? this->difficultyNormalHovered : this->difficultyNormal,
            nullptr,
            this->difficultyNormalRect
        );
        SDL_RenderCopy(
            this->renderer,
            this->hovered == Hard ? this->difficultyHardHovered : this->difficultyHard,
            nullptr,
            this->difficultyHardRect
        );
        SDL_RenderCopy(
            this->renderer,
            this->hovered == Fun ? this->difficultyFunHovered : this->difficultyFun,
            nullptr,
            this->difficultyFunRect
        );
    }

    void MapMenu::handleEvent(SDL_Event* event) {
        if (!this->displayed) return;
        if (event->type == SDL_MOUSEMOTION) {
            IPoint point = {event->motion.x, event->motion.y};
            if (contains(this->arrowLeftRect, point)) this->hovered = Left;
            else if (contains(this->arrowRightRect, point)) this->hovered = Right;
            else if (contains(this->difficultyEasyRect, point)) this->hovered = Easy;
            else if (contains(this->difficultyNormalRect, point)) this->hovered = Normal;
            else if (contains(this->difficultyHardRect, point)) this->hovered = Hard;
            else if (contains(this->difficultyFunRect, point)) this->hovered = Fun;
            else this->hovered = None;
        } else if (event->type == SDL_MOUSEBUTTONUP) {
            if (this->hovered == Left) {
                this->place--;
                while (this->place < 0) this->place += this->mapList.size();
                this->map->loadMap(this->mapList[this->place]);
            } else if (this->hovered == Right) {
                this->place++;
                this->place %= this->mapList.size();
                this->map->loadMap(this->mapList[this->place]);
            } else if (this->hovered != None) this->map->start(this->hovered);
        }
    }

    void MapMenu::setDestRect(SDL_Rect* newDestRect) {
        this->Renderable::setDestRect(newDestRect);
        this->updatePosition();
    }

    void MapMenu::setPosition(const IPoint& position) {
        this->Renderable::setPosition(position);
        this->updatePosition();
    }
    
    void MapMenu::setPosition(const DPoint& position) {
        this->Renderable::setPosition(position);
        this->updatePosition();
    }

    void MapMenu::setDisplayed(bool newDisplayed) {
        this->displayed = newDisplayed;
    }
};
