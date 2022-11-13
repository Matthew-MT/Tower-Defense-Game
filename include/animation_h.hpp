#pragma once
#include <SDL2/SDL.h>
#include "sprite.hpp"
#include <vector>
#include <string.h>

namespace game
{
    class Animation:public Sprite
    {
        protected:
        std::vector<SDL_Texture*> images;
        long currentTime;
        int totalTime;
        SDL_Texture* storedTexture;
        double angle;
        std::string file;
        int frames;
        int millisPerFrame;
        public:
        Animation(
            SDL_Renderer* initRenderer,
            SDL_Texture* initTexture,
            SDL_Rect* initDestRect,
            SDL_Rect* initSourceRect,
            double initAngle,
            std::string fileName,
            int initFrames,
            int initMillisPerFrame
        );

        void tick(double scalar);
        SDL_Texture* getTexture();

    };
}