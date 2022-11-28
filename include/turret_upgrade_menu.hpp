#pragma once
#include "forward.hpp"
#include "turret_upgrade_menu_h.hpp"
#include "utils.hpp"

namespace game {
    TurretUpgradeMenu::TurretUpgradeMenu(
        SDL_Renderer* initRenderer
    ) :
        StaticSprite{
            initRenderer,
            textureFromImageFile(initRenderer, "assets/images/turret_upgrade_menu.png"),
            nullptr,
            nullptr
        },
        sell{textureFromImageFile(initRenderer, "assets/images/turret_sell.png")},
        upgrade{textureFromImageFile(initRenderer, "assets/images/turret_upgrade.png")} {}

    void TurretUpgradeMenu::render() {
        if (!shown) return;
        this->StaticSprite::render();
        if (this->sell != nullptr) SDL_RenderCopy(
            this->renderer,
            this->sell
        );
    }

    void TurretUpgradeMenu::handleEvent(SDL_Event* event) {}

    void TurretUpgradeMenu::loadMenuFor(TurretData* type, const IPoint& center) {}

    void TurretUpgradeMenu::closeMenu() {
        this->shown = false;
    }
};
