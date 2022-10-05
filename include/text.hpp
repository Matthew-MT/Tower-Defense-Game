#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "sprite.hpp"

namespace game {
    class Text : public StaticSprite {
    protected:
        TTF_Font* font;
        std::string text;
    public:
        Text(
            SDL_Renderer* initRenderer,
            const std::string& fontFile,
            SDL_Rect* initDestRect,
            SDL_Rect* initSourceRect = nullptr,
            const std::string& initText = "",
            int fontSize = 24
        ) : StaticSprite{
            initRenderer,
            initDestRect,
            initSourceRect
        }, text{initText} {
            this->setFont(fontFile, fontSize);
            this->setText(this->text);
        }

        void render() {
            SDL_RenderCopy(
                this->renderer,
                this->texture,
                this->sourceRect,
                this->destRect
            );
        }

        void setFont(const std::string& fontFile, int fontSize = 24) {
            this->setFont(TTF_OpenFont(fontFile.c_str(), fontSize));
        }

        void setFont(TTF_Font* font) {
            this->font = font;
        }

        void setText(const std::string& text) {
            this->text = text;
            SDL_Surface* surface = TTF_RenderText_Solid(this->font, this->text.c_str(), {0x1C, 0x1C, 0x1C, 0});
            this->texture = SDL_CreateTextureFromSurface(this->renderer, surface);
            SDL_FreeSurface(surface);
        }

        const std::string& getText() const {
            return this->text;
        }

        TTF_Font* getFont() {
            return this->font;
        }
    };
};
