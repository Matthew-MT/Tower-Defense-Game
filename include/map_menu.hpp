#pragma once
#include <SDL2/SDL.h>
#include "forward.hpp"
#include "sprite.hpp"

namespace game {
    class MapMenu : public Renderable {
    protected:
        SDL_Texture
            * arrow,
            * easyDifficulty,
            * normalDiffuclty,
            * hardDifficulty,
            * funDifficulty;
    public:
        MapMenu(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect
        ) : Renderable{
            initRenderer,
            initDestRect
        } {}
    };
};
