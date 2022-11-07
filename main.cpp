#include <SDL2/SDL.h>
#include "include/game.hpp"

using namespace game;

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
    TTF_Init();

    SDL_DisplayMode display;
    SDL_GetDesktopDisplayMode(0, &display);
    Game* game = new Game(
        "Game",
        createRect(
            IPoint{
                (int)std::floor((double)display.w / 2.f) - 40,
                (int)std::floor((double)display.h / 2.f) - 40
            },
            IPoint{
                80, 80
            }
        )
    );

    SDL_bool loopShouldStop = SDL_FALSE;

    game->run();

    delete game;

    SDL_Quit();
    return 0;
}