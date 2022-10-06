#pragma once
#include <SDL2/SDL.h>

namespace game {
    class GameState {
    protected:
        int
            maxHealth,
            health,
            cash;
    public:
        GameState(
            int initHealth,
            int initCash
        ) :
            maxHealth{initHealth},
            health{initHealth},
            cash{initCash} {}
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

        bool buy(int amount) {
            if (amount > this->cash) return false;
            this->cash -= amount;
            return true;
        }

        void earn(int amount) {
            this->cash += amount;
        }
    };
};
