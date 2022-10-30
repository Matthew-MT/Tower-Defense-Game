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

namespace game {
    class Game {
    protected:
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        // SDL_Thread
        //     * renderThread = nullptr,
        //     * tickThread = nullptr;
        TTF_Font* font;
        GameState* gameState;
        Music* music;
        SDL_bool
            exit = SDL_FALSE,
            ready = SDL_FALSE;
        std::unordered_set<SDL_Texture*> textureList;
        std::vector<Renderable*> renderList;
        std::vector<std::string> mapProgression;
        Uint64 lastTick = 0;
        // GUI* gui;
        std::string title;
        SDL_Rect* windowRect;

        // static int renderThreadFn(void* thisArg) {
        //     Game* game = (Game*)thisArg;
        //     while (!game->exited()) {
        //         game->renderWindow();
        //         SDL_Delay(100);
        //     }
        //     game->ready = SDL_FALSE;
        //     return 0;
        // }

        // static int tickThreadFn(void* thisArg) {
        //     Game* game = (Game*)thisArg;
        //     while (!game->exited()) {
        //         game->tick();
        //         SDL_Delay(100);
        //     }
        //     return 0;
        // }
    public:
        Game(
            const std::string& initTitle,
            SDL_Rect* initWindowRect
        ) :
            title{initTitle},
            windowRect{initWindowRect} {
            std::ifstream
                mapProgressionFile("assets/config/map_progression.txt", std::ios_base::in);
            std::string buffer;

            while (std::getline(mapProgressionFile, buffer)) this->mapProgression.push_back(buffer);

            this->font = TTF_OpenFont("assets/fonts/arial.ttf", 20);

            this->window = SDL_CreateWindow(this->title.c_str(), this->windowRect->x, this->windowRect->y, this->windowRect->w, this->windowRect->h, 0);
            this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);

            SDL_Rect* mapRect = new SDL_Rect();
            mapRect->x = 20;
            mapRect->y = 20;
            mapRect->w = -1;
            mapRect->h = -1;

            Map* map = new Map(
                this->renderer,
                this->font,
                mapRect,
                {40, 40},
                "Tower Defense"
            );

            this->gameState = map->loadMap(this->mapProgression.front());
            this->renderList.push_back(map);
            mapRect = map->getDestRect();

            IPoint winSize = {
                mapRect->w + 40,
                mapRect->h + 80
            };

            SDL_SetWindowSize(this->window, winSize.x, winSize.y);

            TurretHandler* turret = new TurretHandler(
                this->renderer,
                map->getDestRect(),
                map
            );

            this->renderList.push_back(turret);

            this->music = new Music("assets/sound/Industrial Revolution.mp3");
            this->music->playMusic();

            delete mapRect;
            // this->tickThread = SDL_CreateThread(this->tickThreadFn, "tick", this);
        }

        ~Game() {
            // delete this->gui;
            delete this->music;
            this->exit = SDL_TRUE;
            int status;
            // if (this->renderThread != nullptr) SDL_WaitThread(this->renderThread, &status);
            // if (this->tickThread != nullptr) SDL_WaitThread(this->tickThread, &status);
            for (SDL_Texture* texture : this->textureList) SDL_DestroyTexture(texture);
            this->textureList.clear();
            for (Renderable* renderable : this->renderList) delete renderable;
            this->renderList.clear();
            SDL_DestroyRenderer(this->renderer);
            SDL_DestroyWindow(this->window);
        }

        virtual void run() {
            // this->renderThread = SDL_CreateThread(this->renderThreadFn, "render", this);
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
            // SDL_WaitThread(this->renderThread, &status);
            // this->renderThread = nullptr;
            // SDL_Log(("Render thread exited with status " + std::to_string(status) + ".").c_str());
            // SDL_WaitThread(this->tickThread, &status);
            // this->tickThread = nullptr;
            // SDL_Log(("Tick thread exited with status " + std::to_string(status) + ".").c_str());
        }

        virtual void renderWindow() {
            this->tick();
            SDL_RenderClear(this->renderer);
            for (Renderable* renderable : this->renderList) renderable->render();
            // this->gui->render();
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
            if (this->gameState->isDead()) {}
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
