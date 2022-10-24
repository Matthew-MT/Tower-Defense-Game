#pragma once
#include <SDL2/SDL.h>
#include "game_state_h.hpp"

namespace game {
    GameState::GameState(
        SDL_Renderer* initRenderer,
        SDL_Rect* initDestRect,
        int initHealth,
        int initCash
    ) :
        StaticSprite{
            initRenderer,
            initDestRect
        },
        maxHealth{initHealth},
        health{initHealth},
        cash{initCash} {}
    GameState::~GameState() {}

    void GameState::render() {}

    void GameState::reduceHealth(int amount) {
        this->health -= amount;
    }

    void GameState::increaseHealth(int amount) {
        this->health += amount;
    }

    void GameState::resetHealth() {
        this->health = this->maxHealth;
    }

    void GameState::zeroHealth() {
        this->health = 0;
    }

    int GameState::getHealth() {
        return this->health;
    }

    bool GameState::isDead() {
        return health <= 0;
    }

    bool GameState::buy(int amount) {
        if (amount > this->cash) return false;
        this->cash -= amount;
        return true;
    }

    void GameState::earn(int amount) {
        this->cash += amount;
    }
};
