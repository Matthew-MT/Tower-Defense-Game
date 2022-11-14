#pragma once
#include <SDL2/SDL.h>
#include "sprite.hpp"
#include <vector>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include "animation_h.hpp"

namespace game
{
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
            frames++;
            for(int i=2; i<=frames;i++)
            {
                file = baseFile + std::to_string(i) + ".bmp";
                SDL_Log(file.c_str());
                SDL_Surface* surface = SDL_LoadBMP(file.c_str());
                SDL_Texture* loadTexture = SDL_CreateTextureFromSurface(this->renderer, surface);
                SDL_FreeSurface(surface);
                images.push_back(loadTexture);
                totalTime+=millisPerFrame;
            }
            
        }
        
        void Animation::tick(double scalar)
        {
            unsigned current = rand()%(frames+1);
            if(current>=images.size() || current > frames) current = 0;
            storedTexture = images[current];
        }

        SDL_Texture* Animation::getTexture()
        {
            return storedTexture;
        }

}