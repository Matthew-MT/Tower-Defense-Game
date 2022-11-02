#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "forward.hpp"
#include "sprite.hpp"
#include <vector>

namespace game {
    class MapMenu : public Renderable {
    public:
        enum Option {
            None = 0,
            Right = 1,
            Left = 2,
            Easy = 3,
            Normal = 4,
            Hard = 5,
            Fun = 6
        };
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
            * arrowRightRect,
            * arrowLeftRect,
            * difficultyEasyRect,
            * difficultyNormalRect,
            * difficultyHardRect,
            * difficultyFunRect;
        Option hovered = Option::None;

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
    };
};
