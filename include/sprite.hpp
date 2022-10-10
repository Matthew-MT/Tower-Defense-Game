#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include "utils.hpp"

namespace game {
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

        virtual void render() {}

        virtual void tick(double scalar) {}

        void setRenderer(SDL_Renderer* renderer) {
            this->renderer = renderer;
        }

        void setDestRect(SDL_Rect* newDestRect) {
            delete this->destRect;
            this->destRect = newDestRect;
        }

        void setPosition(const IPoint& position) {
            this->destRect->x = position.x;
            this->destRect->y = position.y;
        }

        const SDL_Renderer* getRenderer() const {
            return this->renderer;
        }

        const SDL_Rect* getDestRect() const {
            return this->destRect;
        }

        IPoint getSize() {
            return {this->destRect->w, this->destRect->h};
        }

        IPoint getPosition() const {
            return {this->destRect->x, this->destRect->y};
        }

        void handleEvent(SDL_Event* event) {}
    };

    class StaticSprite : public Renderable {
    protected:
        SDL_Texture* texture;
        SDL_Rect* sourceRect;
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
            delete this->sourceRect;
            this->sourceRect = newSourceRect;
        }

        const SDL_Rect* getSourceRect() const {
            return this->sourceRect;
        }
    };

    class Sprite : public StaticSprite {
    protected:
        double angle;
    public:
        Sprite(
            SDL_Renderer* initRenderer,
            SDL_Texture* initTexture,
            SDL_Rect* initDestRect,
            SDL_Rect* initSourceRect = nullptr,
            double initAngle = 0.f
        ) : StaticSprite{
            initRenderer,
            initTexture,
            initDestRect,
            initSourceRect
        }, angle{initAngle} {}

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
