#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>

namespace game{
    class Sound{
    protected:
        Mix_Chunk* sound;
    public:
        Sound(const std::string& filename) {
            this->sound = Mix_LoadWAV(filename.c_str());
        }
        Mix_Chunk* getSound() {
            return this->sound;
        }
        void playSound(){
            Mix_PlayChannel(-1, this->sound, 0);
        }
    };
};