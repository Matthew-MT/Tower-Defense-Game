#pragma once
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include "forward.hpp"
#include "sprite.hpp"
#include <vector>

namespace game {
    class MapMenu : public Renderable {
    protected:
        Map* map;
        std::vector<std::string> mapList;
        int place = 0;
        SDL_Texture
            * arrow,
            * arrowHovered,
            * difficultyEasy,
            * difficultyEasyHovered,
            * difficultyNormal,
            * difficultyNormalHovered,
            * difficultyHard,
            * difficultyHardHovered,
            * difficultyFun,
            * difficultyFunHovered;
        SDL_Rect
            * arrowRightRect = nullptr,
            * arrowLeftRect = nullptr,
            * difficultyEasyRect = nullptr,
            * difficultyNormalRect = nullptr,
            * difficultyHardRect = nullptr,
            * difficultyFunRect = nullptr;
        Option hovered = Option::None;
        bool displayed = true;

        virtual void updatePosition();
    public:
        MapMenu(
            SDL_Renderer* initRenderer,
            SDL_Rect* initDestRect,
            Map* initMap,
            const std::vector<std::string>& initMapList
        );

        ~MapMenu();

        virtual void render();
        virtual void handleEvent(SDL_Event* event);

        virtual void setDestRect(SDL_Rect* newDestRect);
        virtual void setPosition(const IPoint& position);
        virtual void setPosition(const DPoint& position);
        virtual void setDisplayed(bool newDisplayed);
    };
};
