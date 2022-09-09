#include <SDL2/SDL.h>
#include "include/game.hpp"

int main(int argc, char *argv[]) {
    SDL_Window *win = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *bitmapTex = NULL;
    SDL_Surface *bitmapSurface = NULL;
    int posX = 100, posY = 100, width = 320, height = 240;
    SDL_bool loopShouldStop = SDL_FALSE;

    SDL_Init(SDL_INIT_VIDEO);

    win = SDL_CreateWindow("Hello World", posX, posY, width, height, 0);

    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    bitmapSurface = SDL_LoadBMP("img/hello.bmp");
    bitmapTex = SDL_CreateTextureFromSurface(renderer, bitmapSurface);
    SDL_FreeSurface(bitmapSurface);

    SDL_QueryTexture(bitmapTex, NULL, NULL, &width, &height);
    SDL_SetWindowSize(win, width / 2, height / 2);

    while (!loopShouldStop) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    loopShouldStop = SDL_TRUE;
                    break;
                }
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bitmapTex, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(bitmapTex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);

    SDL_Quit();
    return 0;
}
