#pragma once
#include <SDL2/SDL.h>
#include <forward_list>
#include <iostream>
#include <string>

namespace game {
    template<typename Type> class Point {
    public:
        Type x, y;
        Point(Type ix, Type iy) : x{ix}, y{iy} {}
    };

    using IPoint = Point<int>;

    class StaticSprite {
    protected:
        SDL_Renderer* renderer;
        SDL_Texture* texture;
        SDL_Rect* sourceRect = nullptr;
        SDL_Rect* destRect = nullptr;
    public:
        StaticSprite(
            SDL_Renderer* initRenderer,
            SDL_Texture* initTexture,
            SDL_Rect* initSourceRect,
            SDL_Rect* initDestRect
        ) :
            renderer{initRenderer},
            texture{initTexture},
            sourceRect{initSourceRect},
            destRect{initDestRect} {}
        ~StaticSprite() {}
        void render() {
            if (this->renderer == nullptr) std::cout << "Warning: Detected a failed renderer association. Make sure to specify a renderer.\n";
            SDL_RenderCopy(
                this->renderer,
                this->texture,
                this->sourceRect,
                this->destRect
            );
        }
        void setRenderer(SDL_Renderer* renderer) {
            this->renderer = renderer;
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
        const SDL_Rect* getSourceRect() {
            return this->sourceRect;
        }
        const SDL_Rect* getDestRect() {
            return this->destRect;
        }
        IPoint getPosition() {
            return {this->destRect->x, this->destRect->y};
        }
    };

    class Sprite : public StaticSprite {
    protected:
        double angle = 0.f;
    public:
        Sprite(
            SDL_Renderer* initRenderer,
            SDL_Texture* initTexture,
            SDL_Rect* initSourceRect,
            SDL_Rect* initDestRect
        ) : StaticSprite{initRenderer, initTexture, initSourceRect, initDestRect} {}
        ~Sprite() {}
        void render() {
            if (this->renderer == nullptr) std::cout << "Warning: Detected a failed renderer association. Make sure to specify a renderer.\n";
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
        void setAngle(double angle) {
            this->angle = angle;
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
        std::forward_list<Sprite> renderList;
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
        template<typename Type = Sprite> Type& spawn(
            SDL_Texture* initTexture,
            SDL_Rect* initSourceRect,
            SDL_Rect* initDestRect
        ) {
            if (!std::is_base_of<Sprite, Type>::value) throw "Must request a sprite or derivative.";
            Type sprite{this->renderer, initTexture, initSourceRect, initDestRect};
            this->renderList.push_front(sprite);
            return sprite;
        }
        Sprite& spawn(Sprite& sprite) {
            sprite.setRenderer(this->renderer);
            this->renderList.push_front(sprite);
            return sprite;
        }
        SDL_Renderer* getRenderer() {
            return this->renderer;
        }
    };
};
