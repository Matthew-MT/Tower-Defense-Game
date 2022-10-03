#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "sprite.hpp"

namespace game {
    class Text : public StaticSprite {
    protected:
        TTF_Font* font;
        SDL_Surface* surface;
        std::string
            fontFile,
            text;
        int fontSize;
    public:
        Text(
            SDL_Renderer* initRenderer,
            const std::string& initFontFile,
            const std::string& initText,
            SDL_Rect* initDestRect,
            SDL_Rect* initSourceRect = nullptr,
            int initFontSize = 24
        ) : StaticSprite{
            initRenderer,
            initDestRect
        },
            fontFile{initFontFile},
            text{initText},
            fontSize{initFontSize} {
            this->font = TTF_OpenFont(this->fontFile.c_str(), this->fontSize);
            this->surface = TTF_RenderText_Solid(this->font, this->text.c_str(), {0x1C, 0x1C, 0x1C, 0});
            this->texture = SDL_CreateTextureFromSurface(this->renderer, this->surface);
        }
    };
};
