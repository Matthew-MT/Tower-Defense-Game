#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <unordered_set>
#include "sprite.hpp"
#include "turret.hpp"
#include "text.hpp"
#include "map.hpp"
#include "gui.hpp"
#include <string>
#include <vector>
#include "sound.hpp"
#include "music.hpp"
#include "animation.hpp"

namespace game {
    class Game {
    protected:
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        TTF_Font* font;
        GameState* gameState = nullptr;
        Music* music;
        SDL_bool
            exit = SDL_FALSE,
            ready = SDL_FALSE;
        std::unordered_set<SDL_Texture*> textureList;
        std::vector<Renderable*> renderList;
        Uint64 lastTick = 0;
        std::string title;
        SDL_Rect* windowRect;
    public:
        Game(
            const std::string& initTitle,
            SDL_Rect* initWindowRect
        ) :
            title{initTitle},
            windowRect{initWindowRect} {
            this->font = TTF_OpenFont("assets/fonts/arial.ttf", 20);

            this->window = SDL_CreateWindow(this->title.c_str(), this->windowRect->x, this->windowRect->y, this->windowRect->w, this->windowRect->h, 0);
            this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);

            SDL_Rect* mapRect = new SDL_Rect();
            mapRect->x = 20;
            mapRect->y = 0;
            mapRect->w = -1;
            mapRect->h = -1;

            Map* map = new Map(
                this->renderer,
                this->font,
                mapRect,
                {40, 40},
                "Tower Defense"
            );

            this->renderList.push_back(map);
            mapRect = map->getDestRect();

            IPoint winSize = {
                mapRect->w + 40,
                mapRect->h
            };

            SDL_SetWindowSize(this->window, winSize.x, winSize.y);

            this->music = new Music("assets/sounds/Industrial Revolution.mp3");
            this->music->playMusic();


            delete mapRect;
        }

        ~Game() {
            delete this->music;
            this->exit = SDL_TRUE;
            int status;
            for (SDL_Texture* texture : this->textureList) SDL_DestroyTexture(texture);
            this->textureList.clear();
            for (Renderable* renderable : this->renderList) delete renderable;
            this->renderList.clear();
            SDL_DestroyRenderer(this->renderer);
            SDL_DestroyWindow(this->window);
        }

        virtual void run() {
            SDL_Event event;
            while (!this->exited()) {
                this->renderWindow();
                while (SDL_PollEvent(&event)) {
                    switch (event.type) {
                        case SDL_QUIT: {
                            this->exit = SDL_TRUE;
                            this->music->haltMusic();
                            break;
                        }
                        default: {
                            this->handleEvent(&event);
                            break;
                        }
                    }
                }
                SDL_Delay(10);
            }
            int status;
        }

        virtual void renderWindow() {
            this->tick();
            SDL_RenderClear(this->renderer);
            for (Renderable* renderable : this->renderList) renderable->render();
            SDL_RenderPresent(this->renderer);
        }

        template<typename Type = Renderable> Type& spawn(
            SDL_Texture* initTexture,
            SDL_Rect* initSourceRect,
            SDL_Rect* initDestRect
        ) {
            if (!std::is_base_of<Renderable, Type>::value) {
                SDL_Log("`Game::spawn` must request a Renderable or derivative.");
                throw 1;
            }
            Type* renderable = new Type(this->renderer, initTexture, initSourceRect, initDestRect);
            this->renderList.push_back(renderable);
            return renderable;
        }

        virtual Renderable* spawn(Renderable* renderable) {
            renderable->setRenderer(this->renderer);
            this->renderList.push_back(renderable);
            return renderable;
        }

        virtual void tick() {
            const Uint64 nextTick = SDL_GetTicks64();
            const double scalar = (double)(nextTick - this->lastTick) / 1000.f;
            this->lastTick = nextTick;
            for (Renderable* renderable : this->renderList) renderable->tick(scalar);
            if (this->gameState != nullptr && this->gameState->isDead()) {}
        }

        virtual void handleEvent(SDL_Event* event) {
            for (Renderable* renderable : this->renderList) {
                renderable->handleEvent(event);
            }
        }

        virtual SDL_Window* getWindow() {
            return this->window;
        }

        virtual SDL_Renderer* getRenderer() {
            return this->renderer;
        }

        virtual TTF_Font* getFont() {
            return this->font;
        }

        virtual bool exited() {
            return this->exit;
        }

        virtual std::string getTitle() {
            return this->title;
        }

        virtual SDL_Rect* getWindowRect() {
            return this->windowRect;
        }
    };
};
