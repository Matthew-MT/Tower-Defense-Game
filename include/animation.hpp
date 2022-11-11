#pragma once
#include <SDL2/SDL.h>
#include "sprite.hpp"
#include <vector>
#include <string.h>
#include "animation_h.hpp"

namespace game
{
    AnimationFrame::AnimationFrame(
        SDL_Texture *initTexture, 
        int initTime):
        texture{initTexture},
        time{initTime}{}



    Animation::Animation(
            SDL_Renderer* initRenderer,
            SDL_Texture* initTexture,
            SDL_Rect* initDestRect,
            SDL_Rect* initSourceRect,
            double initAngle,
            std::string fileName,
            int initFrames,
            int initMillisPerFrame
        ):Sprite {
            initRenderer,
            initTexture,
            initDestRect,
            initSourceRect,
            initAngle
        },
        file{fileName},
        millisPerFrame{initMillisPerFrame},
        frames{initFrames}

        {
            std::string baseFile = file;
            for(int i=2; i<=7;i++)
            {
                file = baseFile + std::to_string(i) + ".bmp";
                SDL_Log(file.c_str());
                SDL_Surface* surface = SDL_LoadBMP(file.c_str());
                storedTexture = SDL_CreateTextureFromSurface(this->renderer, surface);
                SDL_FreeSurface(surface);
                images.push_back(AnimationFrame(storedTexture, millisPerFrame));
                totalTime+=millisPerFrame;
            }
            currentTime = rand()%600;
            
        }
        
        void Animation::tick(double scalar)
        {
            int millis = SDL_GetTicks();
            unsigned current = currentTime/50;
            if(current>=images.size()) current = 0;
            this->storedTexture = images[current].texture;
            this->render();
            currentTime =(currentTime+millis)%totalTime;
        }

        SDL_Texture* Animation::getTexture()
        {
            return this->storedTexture;
        }

}