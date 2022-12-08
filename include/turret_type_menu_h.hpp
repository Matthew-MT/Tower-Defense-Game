#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "forward.hpp"
#include "sprite.hpp"
#include <iostream>
#include <fstream>
#include <vector>

namespace game{
    class TurretTypeMenu : public Renderable {
    protected:
        std::vector<TurretData*> options;
        std::vector<SDL_Rect*> optionRects;
        IPoint optionSize;
        int selected = 0;

        virtual IPoint getOptionCenterByIndex(int index);
        virtual void updatePosition();
    public:
        TurretTypeMenu(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect,
            const std::vector<TurretData*>& initOptions,
            const IPoint& initOptionSize
        );
        ~TurretTypeMenu();
        virtual void render();
        virtual void handleEvent(SDL_Event* event);

        virtual void setDestRect(SDL_Rect* newDestRect);
        virtual void setPosition(const IPoint& position);
        virtual void setPosition(const DPoint& position);
        virtual int getSelectedType();
    };
};