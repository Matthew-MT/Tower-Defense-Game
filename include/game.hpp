#pragma once
#include <SDL2/SDL.h>
#include <string>

namespace game {
    class Game {
    private:
        SDL_bool exit = SDL_FALSE;
        SDL_Window* win = nullptr;
        SDL_Renderer* renderer = nullptr;
    public:
        Game(const std::string& title, int x, int y, int w, int h);
    };
};
