#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "forward.hpp"
#include "sprite.hpp"

namespace game {
    class MapMenu : public Renderable {
    public:
        enum Option {
            None = 0,
            Right = 1,
            Left = 2,
            Easy = 3,
            Normal = 4,
            Hard = 5,
            Fun = 6
        };
    protected:
        SDL_Texture
            * arrow,
            * arrowHovered,
            * difficultyEasy,
            * difficultyEasyHovered,
            * difficultyNormal,
            * difficultyNormalHovered,
            * difficultyHard,
            * difficultyHardHovered,
            * difficultyFun,
            * difficultyFunHovered;
        SDL_Rect
            * arrowRightRect,
            * arrowLeftRect,
            * difficultyEasyRect,
            * difficultyNormalRect,
            * difficultyHardRect,
            * difficultyFunRect;
        Option hovered = Option::None;

        virtual void updatePosition() {}
    public:
        MapMenu(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect
        ) : Renderable{
            initRenderer,
            initDestRect
        } {
            SDL_Surface* surface = IMG_Load("assets/images/arrow_right.png");
            this->arrow = SDL_CreateTextureFromSurface(this->renderer, surface);
            SDL_FreeSurface(surface);
            surface = IMG_Load("assets/images/arrow_right_hovered.png");
            this->arrowHovered = SDL_CreateTextureFromSurface(this->renderer, surface);
            SDL_FreeSurface(surface);
            surface = IMG_Load("assets/images/difficulty_easy.png");
            this->difficultyEasy = SDL_CreateTextureFromSurface(this->renderer, surface);
            SDL_FreeSurface(surface);
            surface = IMG_Load("assets/images/difficulty_easy_hovered.png");
            this->difficultyEasyHovered = SDL_CreateTextureFromSurface(this->renderer, surface);
            SDL_FreeSurface(surface);
            surface = IMG_Load("assets/images/difficulty_normal.png");
            this->difficultyNormal = SDL_CreateTextureFromSurface(this->renderer, surface);
            SDL_FreeSurface(surface);
            surface = IMG_Load("assets/images/difficulty_normal_hovered.png");
            this->difficultyNormalHovered = SDL_CreateTextureFromSurface(this->renderer, surface);
            SDL_FreeSurface(surface);
            surface = IMG_Load("assets/images/difficulty_hard.png");
            this->difficultyHard = SDL_CreateTextureFromSurface(this->renderer, surface);
            SDL_FreeSurface(surface);
            surface = IMG_Load("assets/images/difficulty_hard_hovered.png");
            this->difficultyHardHovered = SDL_CreateTextureFromSurface(this->renderer, surface);
            SDL_FreeSurface(surface);
            surface = IMG_Load("assets/images/difficulty_fun.png");
            this->difficultyFun = SDL_CreateTextureFromSurface(this->renderer, surface);
            SDL_FreeSurface(surface);
            surface = IMG_Load("assets/images/difficulty_fun_hovered.png");
            this->difficultyFunHovered = SDL_CreateTextureFromSurface(this->renderer, surface);
            SDL_FreeSurface(surface);
        }

        ~MapMenu() {
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

        virtual void render() {
            SDL_RenderCopy(
                this->renderer,
                this->hovered == Option::Right ? this->arrowHovered : this->arrow,
                nullptr,
                this->arrowRightRect
            );
            SDL_RenderCopyEx(
                this->renderer,
                this->hovered == Option::Left ? this->arrowHovered : this->arrow,
                nullptr,
                this->arrowLeftRect,
                0.f, nullptr,
                SDL_FLIP_HORIZONTAL
            );
            SDL_RenderCopy(
                this->renderer,
                this->hovered == Option::Easy ? this->difficultyEasyHovered : this->difficultyEasy,
                nullptr,
                this->difficultyEasyRect
            );
            SDL_RenderCopy(
                this->renderer,
                this->hovered == Option::Normal ? this->difficultyNormalHovered : this->difficultyNormal,
                nullptr,
                this->difficultyNormalRect
            );
            SDL_RenderCopy(
                this->renderer,
                this->hovered == Option::Hard ? this->difficultyHardHovered : this->difficultyHard,
                nullptr,
                this->difficultyHardRect
            );
            SDL_RenderCopy(
                this->renderer,
                this->hovered == Option::Fun ? this->difficultyFunHovered : this->difficultyFun,
                nullptr,
                this->difficultyFunRect
            );
        }

        virtual void handleEvent(SDL_Event* event) {
            if (event->type == SDL_MOUSEMOTION) {
            }
        }
    };
};
