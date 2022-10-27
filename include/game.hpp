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
        SDL_bool exit = SDL_FALSE;
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        std::unordered_set<SDL_Texture*> textureList;
        std::vector<Renderable*> renderList;
        std::vector<std::string> mapProgression;
        Uint64 lastTick = 0;
        GUI* gui;
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
            GameState* initGameState = map->loadMap(this->mapProgression.front());
            this->renderList.push_back(map);
            mapRect = map->getDestRect();

            IPoint winSize = {
                mapRect->w + 40,
                mapRect->h + 40
            };
            SDL_SetWindowSize(this->window, winSize.x, winSize.y);

            TurretHandler* turret = new TurretHandler(
                this->renderer,
                map->getDestRect(),
                map
            );
            this->renderList.push_back(turret);

            TTF_Font* font = TTF_OpenFont("assets/fonts/SansSerifCollection.ttf", 24);

            Text* cash = new Text(
                this->renderer,
                font,
                {
                    winSize.x - 40,
                    20
                },
                nullptr,
                "Cash: 0"
            );

            this->gui = new GUI(this->window, this->renderer);

            Music* myMusic = new Music("assets/sound/Industrial Revolution.mp3");
            myMusic -> playMusic();
        }

        ~Game() {
            for (SDL_Texture* texture : this->textureList) SDL_DestroyTexture(texture);
            this->textureList.clear();
            for (Renderable* renderable : this->renderList) delete renderable;
            this->renderList.clear();
            SDL_DestroyRenderer(this->renderer);
            SDL_DestroyWindow(this->window);
            delete this->gui;
        }

        virtual void renderWindow() {
            this->tick();
            SDL_RenderClear(this->renderer);
            for (Renderable* renderable : this->renderList) renderable->render();
            this->gui->render();
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

        virtual SDL_Renderer* getRenderer() {
            return this->renderer;
        }

        virtual void tick() {
            const Uint64 nextTick = SDL_GetTicks64();
            const double scalar = (double)(nextTick - this->lastTick) / 1000.f;
            this->lastTick = nextTick;
            for (Renderable* renderable : this->renderList) renderable->tick(scalar);
        }

        virtual void handleEvent(SDL_Event* event) {
            for (Renderable* renderable : this->renderList) {
                renderable->handleEvent(event);
            }
        }
    };
};
