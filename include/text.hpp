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
            TTF_Font* initFont,
            SDL_Rect* initDestRect,
            SDL_Rect* initSourceRect = nullptr,
            const std::string& initText = ""
        ) :
            StaticSprite{
                initRenderer,
                initDestRect,
                initSourceRect
            },
            text{initText},
            font{initFont} {
            this->setText(this->text);
        }

        Text(
            SDL_Renderer* initRenderer,
            const std::string& fontFile,
            SDL_Rect* initDestRect,
            SDL_Rect* initSourceRect = nullptr,
            const std::string& initText = "",
            int fontSize = 24
        ) :
            Text{
                initRenderer,
                TTF_OpenFont(fontFile.c_str(), fontSize),
                initDestRect,
                initSourceRect,
                initText
            } {}
        
        Text(
            SDL_Renderer* initRenderer,
            TTF_Font* initFont,
            const IPoint& position,
            SDL_Rect* initSourceRect = nullptr,
            const std::string& initText = ""
        ) :
            Text{
                initRenderer,
                initFont,
                createRect(position, IPoint{1, 1}),
                initSourceRect,
                initText
            } {}
        
        Text(
            SDL_Renderer* initRenderer,
            const std::string& fontFile,
            const IPoint& position,
            SDL_Rect* initSourceRect = nullptr,
            const std::string& initText = "",
            int fontSize = 24
        ) :
            Text{
                initRenderer,
                TTF_OpenFont(fontFile.c_str(), fontSize),
                position,
                initSourceRect,
                initText
            } {}

        void render() {
            SDL_RenderCopy(
                this->renderer,
                this->texture,
                this->sourceRect,
                this->destRect
            );
        }

        void setFont(TTF_Font* font) {
            this->font = font;
        }

        void setFont(const std::string& fontFile, int fontSize = 24) {
            this->setFont(TTF_OpenFont(fontFile.c_str(), fontSize));
        }

        void setText(const std::string& text) {
            this->text = text;
            SDL_Surface* surface = TTF_RenderText_Solid(this->font, this->text.c_str(), {0x1C, 0x1C, 0x1C, 0});
            this->texture = SDL_CreateTextureFromSurface(this->renderer, surface);
            SDL_FreeSurface(surface);
        }

        virtual void setPosition(const IPoint& position) {
            int w, h;
            SDL_QueryTexture(this->texture, nullptr, nullptr, &w, &h);
            this->destRect->x = position.x - (w >> 1);
            this->destRect->y = position.y - (h >> 1);
            this->destRect->w = w;
            this->destRect->h = h;
        }

        const std::string& getText() const {
            return this->text;
        }

        TTF_Font* getFont() {
            return this->font;
        }
    };
};
