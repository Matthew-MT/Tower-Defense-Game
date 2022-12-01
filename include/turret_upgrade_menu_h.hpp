#pragma once
#include <SDL2/SDL_ttf.h>
#include "forward.hpp"
#include "turret_h.hpp"
#include "sprite.hpp"
#include "enums.hpp"
#include "text.hpp"

namespace game {
    class TurretUpgradeMenu : public StaticSprite {
    protected:
        SDL_Texture
            * sell = nullptr,
            * sellHovered = nullptr,
            * upgrade = nullptr,
            * upgradeHovered = nullptr,
            * upgradeDisabled = nullptr;
        Text
            * sellText = nullptr,
            * upgradeText = nullptr;
        SDL_Rect
            * sellRect = nullptr,
            * upgradeRect = nullptr;
        Map* map;
        TTF_Font* font;
        IPoint currentCenter = {0, 0};
        bool
            shown = false,
            canUpgrade = false;
        SingleOption hovered = Neither;
        Turret* menuTarget = nullptr;

        virtual void updateRects();
    public:
        TurretUpgradeMenu(
            SDL_Renderer* initRenderer,
            Map* initMap,
            TTF_Font* initFont
        );

        virtual void setDestRect(SDL_Rect* newDestRect);
        virtual void setPosition(const IPoint& position);
        virtual void setPosition(const DPoint& position);

        virtual void render();
        virtual void handleEvent(SDL_Event* event);
        virtual void loadMenuFor(Turret* target, const IPoint& index);
        virtual void closeMenu();
    };
};
