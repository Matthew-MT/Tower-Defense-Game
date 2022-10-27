#pragma once

/*
    Music is Industrial Revolution by Kevin MacLeod, License: CC BY 3.0 (http://goo.gl/BlcHZR)
*/

#include <SDL2/SDL.h>
#include <SDL_mixer.h>
#include <iostream>

namespace game{
    class Music{
    protected:
        Mix_Music* music;
    public:
        Music(const std::string& filename) {
            this->music = Mix_LoadMUS(filename.c_str());
        }
        Mix_Music* getMusic() {
            return this->music;
        }
        void playMusic(){
            Mix_PlayMusic(this->music, -1);
        }
    };
};