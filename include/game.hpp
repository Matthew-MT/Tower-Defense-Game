#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <vector>

namespace game {
    template<typename Type> class Point {
    public:
        Type x, y;
        Point(Type ix, Type iy) : x{ix}, y{iy} {}
    };

    using IPoint = Point<int>;

    class Sprite {
    private:
        SDL_Renderer* renderer;
        SDL_Texture* texture;
        SDL_Rect* sourceRect = nullptr;
        SDL_Rect* destRect = nullptr;
        double angle = 0.f;
    public:
        Sprite(
            SDL_Renderer* initRenderer,
            SDL_Texture* initTexture,
            SDL_Rect* initSourceRect,
            SDL_Rect* initDestRect
        ) :
            renderer{initRenderer},
            texture{initTexture},
            sourceRect{initSourceRect},
            destRect{initDestRect} {}
        ~Sprite() {}
        void render() {
            SDL_RenderCopyEx(
                this->renderer,
                this->texture,
                this->sourceRect,
                this->destRect,
                this->angle,
                nullptr,
                SDL_FLIP_NONE
            );
        }
        void setTexture(SDL_Texture* texture) {
            this->texture = texture;
        }
        void setSourceRect(SDL_Rect* newSourceRect) {
            this->sourceRect = newSourceRect;
        }
        void setDestRect(SDL_Rect* newDestRect) {
            this->destRect = newDestRect;
        }
        void setPosition(int x, int y) {
            this->destRect->x = x;
            this->destRect->y = y;
        }
        void setAngle(double angle) {
            this->angle = angle;
        }
        const SDL_Rect* getSourceRect() {
            return this->sourceRect;
        }
        const SDL_Rect* getDestRect() {
            return this->destRect;
        }
        IPoint getPosition() {
            return {this->destRect->x, this->destRect->y};
        }
        double getAngle(double angle) {
            return this->angle;
        }
    };

    class Game {
    private:
        SDL_bool exit = SDL_FALSE;
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        std::vector<Sprite> renderList;
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
            for (Sprite sprite : this->renderList) sprite.render();
            SDL_RenderPresent(this->renderer);
        }
        void spawnSprite(Sprite& sprite) {
            this->renderList.push_back(sprite);
        }
    };
};
