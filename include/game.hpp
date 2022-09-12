#pragma once
#include <SDL2/SDL.h>
#include <string>

namespace game {
    class RenderableTexture {
    private:
        SDL_Texture* texture;
        SDL_Rect* sourceRect = nullptr;
        SDL_Rect* destRect = nullptr;
    public:
        RenderableTexture() {}
        ~RenderableTexture() {}
        void render(SDL_Renderer* renderer) {
            SDL_RenderCopy(renderer, this->texture, this->sourceRect, this->destRect);
        }
        void setSourceRect(int x, int y, int w, int h) {}
    };

    class Game {
    private:
        SDL_bool exit = SDL_FALSE;
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
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
        }
    };
};
