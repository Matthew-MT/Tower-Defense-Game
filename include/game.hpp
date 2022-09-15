#pragma once
#include <SDL2/SDL.h>
#include <forward_list>
#include "sprite.hpp"
#include <string>

namespace game {
    class Game {
    private:
        SDL_bool exit = SDL_FALSE;
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        std::forward_list<Sprite> renderList;
    public:
        Game(const std::string& title, int x, int y, int w, int h) {
            this->window = SDL_CreateWindow(title.c_str(), x, y, w, h, 0);
            this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
        }

        ~Game() {
            SDL_DestroyWindow(this->window);
        }

        void render() {
            SDL_RenderClear(this->renderer);
            for (Sprite sprite : this->renderList) sprite.render();
            SDL_RenderPresent(this->renderer);
        }

        template<typename Type = Sprite> Type& spawn(
            SDL_Texture* initTexture,
            SDL_Rect* initSourceRect,
            SDL_Rect* initDestRect
        ) {
            if (!std::is_base_of<Sprite, Type>::value) throw "Must request a sprite or derivative.";
            Type sprite{this->renderer, initTexture, initSourceRect, initDestRect};
            this->renderList.push_front(sprite);
            return sprite;
        }

        Sprite& spawn(Sprite& sprite) {
            sprite.setRenderer(this->renderer);
            this->renderList.push_front(sprite);
            return sprite;
        }

        SDL_Renderer* getRenderer() {
            return this->renderer;
        }
    };
};
