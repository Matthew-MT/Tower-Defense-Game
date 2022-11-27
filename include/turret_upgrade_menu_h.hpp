#pragma once
#include "forward.hpp"
#include "sprite.hpp"

namespace game {
    class TurretUpgradeMenu : public StaticSprite {
    protected:
    public:
        TurretUpgradeMenu(
            SDL_Renderer* initRenderer
        );
    };
};
