#pragma once
#include "forward.hpp"
#include "turret_upgrade_menu_h.hpp"
#include "utils.hpp"

namespace game {
    TurretUpgradeMenu::TurretUpgradeMenu(
        SDL_Renderer* initRenderer
    ) : StaticSprite{
        initRenderer,
        textureFromImageFile(initRenderer, "assets/images/turret_upgrade_menu.png"),
        nullptr,
        nullptr
    } {}
};
