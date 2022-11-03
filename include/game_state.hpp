#pragma once
#include <SDL2/SDL.h>
#include "game_state_h.hpp"

namespace game {
    IPoint GameState::getTitleDisplayPosition() {
        return {
            this->destRect->x + (this->destRect->h << 1),
            this->destRect->y + (this->destRect->h >> 1)
        };
    }

    IPoint GameState::getCashDisplayPositon() {
        return {
            this->destRect->x + (this->destRect->w >> 1) + (this->destRect->h >> 1),
            this->destRect->y + (this->destRect->h >> 1)
        };
    }

    IPoint GameState::getHealthDisplayPosition() {
        IPoint calculated = {
            (this->destRect->x + this->destRect->w) - (this->destRect->h << 1),
            this->destRect->y + (this->destRect->h >> 1)
        };
        return calculated;
    }

    std::string GameState::getCashText() {
        return "Cash: " + std::to_string(this->cash);
    }

    std::string GameState::getHealthText() {
        return "Health: " + std::to_string(this->health);
    }

    void GameState::updatePosition() {
        this->titleDisplay->setPosition(this->getTitleDisplayPosition());
        this->cashDisplay->setPosition(this->getCashDisplayPositon());
        this->healthDisplay->setPosition(this->getHealthDisplayPosition());
    }

    GameState::GameState(
        SDL_Renderer* initRenderer,
        TTF_Font* initFont,
        SDL_Rect* initDestRect,
        const std::string& title,
        int initCash,
        int initHealth
    ) :
        StaticSprite{
            initRenderer,
            initDestRect
        },
        maxCash{initCash},
        maxHealth{initHealth},
        cash{initCash},
        health{initHealth} {
        this->titleDisplay = new Text(
            this->renderer,
            initFont,
            this->getTitleDisplayPosition(),
            nullptr,
            title
        );
        this->cashDisplay = new Text(
            this->renderer,
            initFont,
            this->getCashDisplayPositon(),
            nullptr,
            this->getCashText()
        );
        this->healthDisplay = new Text(
            this->renderer,
            initFont,
            this->getHealthDisplayPosition(),
            nullptr,
            this->getHealthText()
        );
    }
    GameState::~GameState() {}

    void GameState::render() {
        this->titleDisplay->render();
        this->cashDisplay->render();
        this->healthDisplay->render();
    }

    void GameState::reset() {
        this->cash = this->maxCash;
        this->health = this->maxHealth;
    }

    void GameState::setDestRect(SDL_Rect* rect) {
        this->StaticSprite::setDestRect(rect);
        this->updatePosition();
    }

    void GameState::setPosition(const IPoint& position) {
        this->StaticSprite::setPosition(position);
        this->updatePosition();
    }

    void GameState::setPosition(const DPoint& position) {
        this->StaticSprite::setPosition(position);
        this->updatePosition();
    }

    bool GameState::buy(int amount) {
        if (amount > this->cash) return false;
        this->cash -= amount;
        this->cashDisplay->setText(this->getCashText());
        return true;
    }

    void GameState::earn(int amount) {
        this->cash += amount;
        this->cashDisplay->setText(this->getCashText());
    }

    void GameState::reduceHealth(int amount) {
        this->health -= amount;
        this->healthDisplay->setText(this->getHealthText());
    }

    void GameState::increaseHealth(int amount) {
        this->health += amount;
        this->healthDisplay->setText(this->getHealthText());
    }

    void GameState::resetHealth() {
        this->health = this->maxHealth;
        this->healthDisplay->setText(this->getHealthText());
    }

    void GameState::zeroHealth() {
        this->health = 0;
        this->healthDisplay->setText(this->getHealthText());
    }

    int GameState::getHealth() {
        return this->health;
    }

    bool GameState::isDead() {
        return health <= 0;
    }
};
