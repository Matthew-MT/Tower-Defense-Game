#pragma once
#include <SDL2/SDL.h>

namespace game {
    class GameState {
    protected:
        int maxHealth;
        int health;
    public:
        GameState(int initHealth) : maxHealth{initHealth}, health{initHealth} {}
        ~GameState() {}

        void reduceHealth(int amount = 1) {
            this->health -= amount;
        }

        void increaseHealth(int amount = 1) {
            this->health += amount;
        }

        void resetHealth() {
            this->health = this->maxHealth;
        }

        void zeroHealth() {
            this->health = 0;
        }

        int getHealth() {
            return this->health;
        }

        bool isDead() {
            return health <= 0;
        }
    };
};
