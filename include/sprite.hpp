#pragma once
#include <SDL2/SDL.h>
#include <iostream>

namespace game {
    template<typename Type> class Point {
    public:
        Type x, y;
        Point(Type ix, Type iy) : x{ix}, y{iy} {}
    };

    using IPoint = Point<int>;

    class Renderable {
    protected:
        SDL_Renderer* renderer;
        SDL_Rect* destRect;
    public:
        Renderable(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect
        ) :
            renderer{initRenderer},
            destRect{initDestRect} {}
        
        ~Renderable() {}

        virtual void render() {};

        void setRenderer(SDL_Renderer* renderer) {
            this->renderer = renderer;
        }

        void setDestRect(SDL_Rect* newDestRect) {
            delete this->destRect;
            this->destRect = newDestRect;
        }

        void setPosition(IPoint position) {
            this->destRect->x = position.x;
            this->destRect->y = position.y;
        }

        const SDL_Renderer* getRenderer() const {
            return this->renderer;
        }

        const SDL_Rect* getDestRect() const {
            return this->destRect;
        }

        IPoint getPosition() const {
            return {this->destRect->x, this->destRect->y};
        }
    };

    class StaticSprite : public Renderable {
    protected:
        SDL_Texture* texture;
        SDL_Rect* sourceRect = new SDL_Rect();
    public:
        StaticSprite(
            SDL_Renderer* initRenderer,
            SDL_Texture* initTexture,
            SDL_Rect* initDestRect,
            SDL_Rect* initSourceRect = nullptr
        ) : Renderable{
            initRenderer,
            initDestRect
        },
            texture{initTexture},
            sourceRect{initSourceRect} {}

        StaticSprite(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect,
            SDL_Rect* initSourceRect = nullptr
        ) : Renderable{
            initRenderer,
            initDestRect
        },
            texture{nullptr},
            sourceRect{initSourceRect} {}

        ~StaticSprite() {}

        void render() {
            if (this->renderer == nullptr) {
                SDL_Log("Warning: Detected a failed renderer association. Make sure to specify a renderer.");
                return;
            }
            SDL_RenderCopy(
                this->renderer,
                this->texture,
                this->sourceRect,
                this->destRect
            );
        }

        void setTexture(SDL_Texture* texture) {
            this->texture = texture;
        }

        void setSourceRect(SDL_Rect* newSourceRect) {
            this->sourceRect = newSourceRect;
        }

        const SDL_Rect* getSourceRect() const {
            return this->sourceRect;
        }
    };

    class Sprite : public StaticSprite {
    protected:
        double angle = 0.f;
    public:
        Sprite(
            SDL_Renderer* initRenderer,
            SDL_Texture* initTexture,
            SDL_Rect* initDestRect,
            SDL_Rect* initSourceRect = nullptr
        ) : StaticSprite{
            initRenderer,
            initTexture,
            initDestRect,
            initSourceRect
        } {}

        Sprite(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect,
            SDL_Rect* initSourceRect
        ) : StaticSprite{
            initRenderer,
            initDestRect,
            initSourceRect
        } {}

        ~Sprite() {}

        void render() {
            if (this->renderer == nullptr) SDL_Log("Warning: Detected a failed renderer association. Make sure to specify a renderer.");
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

        double getAngle() const {
            return this->angle;
        }
    };
};
