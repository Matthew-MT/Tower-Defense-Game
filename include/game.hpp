#pragma once
#include <SDL2/SDL.h>
#include <string>

namespace game {
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
    };
};
