#include <SDL2/SDL.h>
#include "include/game.hpp"
// #include "include/gui.hpp"

using namespace game;

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    Mix_OpenAudio(22500, MIX_DEFAULT_FORMAT, 2, 4096);

    TTF_Init();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    SDL_DisplayMode display;
    SDL_GetDesktopDisplayMode(0, &display);
    Game* game = new Game(
        "Game",
        (int)std::floor((double)display.w / 2.f) - 40,
        (int)std::floor((double)display.h / 2.f) - 40,
        80, 80
    );
    // SDL_Window *win = NULL;
    // SDL_Renderer *renderer = NULL;
    // SDL_Texture *bitmapTex = NULL;
    // SDL_Surface *bitmapSurface = NULL;
    // int posX = 100, posY = 100, width = 320, height = 240;

    SDL_bool loopShouldStop = SDL_FALSE;

    // win = SDL_CreateWindow("Hello World", posX, posY, width, height, 0);

    // renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    // bitmapSurface = SDL_LoadBMP("assets/images/hello.bmp");
    // bitmapTex = SDL_CreateTextureFromSurface(renderer, bitmapSurface);
    // SDL_FreeSurface(bitmapSurface);

    // SDL_QueryTexture(bitmapTex, NULL, NULL, &width, &height);
    // SDL_SetWindowSize(win, width / 2, height / 2);

    while (!loopShouldStop) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            game->handleEvent(&event);
            switch (event.type) {
                case SDL_QUIT: {
                    loopShouldStop = SDL_TRUE;
                    break;
                }
            }
        }

        game->renderWindow();
        SDL_Delay(10);
        // SDL_RenderClear(renderer);
        // SDL_RenderCopy(renderer, bitmapTex, NULL, NULL);
        // SDL_RenderPresent(renderer);
    }

    delete game;
    // SDL_DestroyTexture(bitmapTex);
    // SDL_DestroyRenderer(renderer);
    // SDL_DestroyWindow(win);

    SDL_Quit();
    return 0;
}
