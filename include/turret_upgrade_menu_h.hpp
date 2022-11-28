#pragma once
#include "forward.hpp"
#include "turret_h.hpp"
#include "sprite.hpp"
#include "text.hpp"

namespace game {
    class TurretUpgradeMenu : public StaticSprite {
    protected:
        SDL_Texture
            * sell = nullptr,
            * upgrade = nullptr;
        Text
            * sellText = nullptr,
            * upgradeText = nullptr;
        SDL_Rect
            * sellRect = nullptr,
            * upgradeRect = nullptr;
        bool shown = false;

        virtual void updateRects();
    public:
        TurretUpgradeMenu(
            SDL_Renderer* initRenderer
        );

        virtual void render();
        virtual void handleEvent(SDL_Event* event);
        virtual void loadMenuFor(TurretData* type, const IPoint& center);
        virtual void closeMenu();
    };
};
