#pragma once
#include <SDL2/SDL.h>

namespace game {
    class GameState {
    protected:
        int health;
    public:
        GameState(int initHealth) : health{initHealth} {}
        ~GameState() {}

        void reduceHealth(int amount = 1) {
            this->health -= amount;
        }

        void increaseHealth(int amount = 1) {
            this->health += amount;
        }

        bool isDead() {
            return health <= 0;
        }
    };
};
