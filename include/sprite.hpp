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

        virtual void render() {
            SDL_Log("We should not get here.");
        }

        virtual void tick(double scalar) {}

        virtual void setRenderer(SDL_Renderer* renderer) {
            this->renderer = renderer;
        }

        virtual void setDestRect(SDL_Rect* newDestRect) {
            delete this->destRect;
            this->destRect = newDestRect;
        }

        virtual void setPosition(const IPoint& position) {
            this->destRect->x = position.x;
            this->destRect->y = position.y;
        }

        virtual SDL_Renderer* getRenderer() {
            return this->renderer;
        }

        virtual const SDL_Rect* getDestRect() const {
            return this->destRect;
        }

        virtual IPoint getSize() {
            return {this->destRect->w, this->destRect->h};
        }

        virtual IPoint getPosition() const {
            return {this->destRect->x, this->destRect->y};
        }

        virtual void handleEvent(SDL_Event* event) {}
    };

    class StaticSprite : public Renderable {
    protected:
        SDL_Texture* texture;
        SDL_Rect* sourceRect;
        DPoint position = {0.f, 0.f};
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
            sourceRect{initSourceRect} {
            if (this->destRect != nullptr) this->position = {
                (double)this->destRect->x,
                (double)this->destRect->y
            };
        }

        StaticSprite(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect,
            SDL_Rect* initSourceRect = nullptr
        ) : Renderable{
            initRenderer,
            initDestRect
        },
            texture{nullptr},
            sourceRect{initSourceRect} {
            if (this->destRect != nullptr) this->position = {
                (double)this->destRect->x,
                (double)this->destRect->y
            };
        }

        ~StaticSprite() {}

        virtual void render() {
            if (this->renderer == nullptr) {
                SDL_Log("Warning: Detected a failed renderer association. Make sure to specify a renderer.");
                return;
            }
            SDL_Log("Rendering title...");
            SDL_RenderCopy(
                this->renderer,
                this->texture,
                this->sourceRect,
                this->destRect
            );
        }

        virtual void setDestRect(SDL_Rect* newDestRect) {
            this->position.x = (double)newDestRect->x;
            this->position.y = (double)newDestRect->y;
            this->Renderable::setDestRect(newDestRect);
        }

        virtual void setPosition(const IPoint& position) {
            this->position = (DPoint)position;
            this->Renderable::setPosition(position);
        }

        virtual void setPosition(const DPoint& position) {
            this->position = position;
            this->Renderable::setPosition({
                (int)position.x,
                (int)position.y
            });
        }

        virtual void setTexture(SDL_Texture* texture) {
            this->texture = texture;
        }

        virtual void setSourceRect(SDL_Rect* newSourceRect) {
            delete this->sourceRect;
            this->sourceRect = newSourceRect;
        }

        virtual DPoint getDoublePosition() {
            return this->position;
        }

        virtual const SDL_Rect* getSourceRect() const {
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

        virtual void render() {
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

        virtual void setAngle(double angle) {
            this->angle = angle;
        }

        virtual double getAngle() const {
            return this->angle;
        }
    };
};
