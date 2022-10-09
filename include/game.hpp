#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <unordered_set>
#include "sprite.hpp"
#include "map.hpp"
// #include "gui.hpp"
#include <string>
#include <vector>

namespace game {
    class Game {
    protected:
        SDL_bool exit = SDL_FALSE;
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        std::unordered_set<SDL_Texture*> textureList;
        std::vector<Renderable*> renderList;
        std::vector<std::string> mapProgression;
        Uint64 lastTick = 0;
        // GUI* gui;
    public:
        Game(const std::string& title, int x, int y, int w, int h) {
            std::ifstream
                mapProgressionFile("assets/config/map_progression.txt", std::ios_base::in);
            std::string buffer;

            while (std::getline(mapProgressionFile, buffer)) this->mapProgression.push_back(buffer);

            this->window = SDL_CreateWindow(title.c_str(), x, y, w, h, 0);
            this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);

            SDL_Rect* mapRect = new SDL_Rect();
            mapRect->x = 20;
            mapRect->y = 20;
            mapRect->w = -1;
            mapRect->h = -1;

            Map* map = new Map(
                this->renderer,
                mapRect,
                {40, 40}
            );
            map->loadMap(this->mapProgression.front());
            this->renderList.push_back(map);
            mapRect = map->getDestRect();
            SDL_SetWindowSize(this->window, mapRect->w + 40, mapRect->h + 40);

            TTF_Font* font = TTF_OpenFont("assets/fonts/SansSerifCollection.ttf", 24);

            // this->gui = new GUI(this->window, this->renderer);
        }

        ~Game() {
            for (SDL_Texture* texture : this->textureList) SDL_DestroyTexture(texture);
            this->textureList.clear();
            for (Renderable* renderable : this->renderList) delete renderable;
            this->renderList.clear();
            SDL_DestroyRenderer(this->renderer);
            SDL_DestroyWindow(this->window);
            // delete this->gui;
        }

        void renderWindow() {
            this->tick();
            SDL_RenderClear(this->renderer);
            for (Renderable* renderable : this->renderList) renderable->render();
            SDL_RenderPresent(this->renderer);
            // this->gui->guiWindow();
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

        Renderable* spawn(Renderable* renderable) {
            renderable->setRenderer(this->renderer);
            this->renderList.push_back(renderable);
            return renderable;
        }

        const SDL_Renderer* getRenderer() const {
            return this->renderer;
        }

        void tick() {
            const Uint64 nextTick = SDL_GetTicks64();
            const double scalar = (double)(nextTick - this->lastTick) / 1000.f;
            this->lastTick = nextTick;
        }
    };
};
