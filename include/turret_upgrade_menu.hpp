#pragma once
#include "forward.hpp"
#include "turret_upgrade_menu_h.hpp"
#include "turret_h.hpp"
#include "utils.hpp"

namespace game {
    void TurretUpgradeMenu::updateRects() {
        if (this->sellRect != nullptr) delete this->sellRect;
        if (this->upgradeRect != nullptr) delete this->upgradeRect;

        this->sellRect = fromCenter(
            IPoint{
                this->destRect->x + (this->destRect->w >> 3) + (this->destRect->w >> 4),
                this->destRect->y + (this->destRect->h >> 1)
            },
            IPoint{40, 40}
        );
        this->sellText->setPosition(IPoint{
            this->destRect->x + (this->destRect->w >> 3) + (this->destRect->w >> 4),
            this->destRect->y + (this->destRect->h >> 1)
        });

        this->upgradeRect = fromCenter(
            IPoint{
                this->destRect->x + (this->destRect->w >> 1) + (this->destRect->w >> 2) + (this->destRect->w >> 4),
                this->destRect->y + (this->destRect->h >> 1)
            },
            IPoint{40, 40}
        );
        this->upgradeText->setPosition(IPoint{
            this->destRect->x + (this->destRect->w >> 1) + (this->destRect->w >> 2) + (this->destRect->w >> 4),
            this->destRect->y + (this->destRect->h >> 1)
        });
    }

    TurretUpgradeMenu::TurretUpgradeMenu(
        SDL_Renderer* initRenderer,
        Map* initMap,
        TurretHandler* initTurretHandler,
        TTF_Font* initFont
    ) :
        StaticSprite{
            initRenderer,
            textureFromImageFile(initRenderer, "assets/images/turret_upgrade_menu.png"),
            nullptr,
            nullptr
        },
        sell{textureFromImageFile(initRenderer, "assets/images/turret_sell.png")},
        sellHovered{textureFromImageFile(initRenderer, "assets/images/turret_sell_hovered.png")},
        sellText{new Text(initRenderer, initFont, {0, 0})},
        upgrade{textureFromImageFile(initRenderer, "assets/images/turret_upgrade.png")},
        upgradeHovered{textureFromImageFile(initRenderer, "assets/images/turret_upgrade_hovered.png")},
        upgradeDisabled{textureFromImageFile(initRenderer, "assets/images/turret_upgrade_disabled.png")},
        upgradeText{new Text(initRenderer, initFont, {0, 0})},
        map{initMap},
        turretHandler{initTurretHandler},
        font{initFont} {}

    void TurretUpgradeMenu::setDestRect(SDL_Rect* newDestRect) {
        this->StaticSprite::setDestRect(newDestRect);
        this->updateRects();
    }

    void TurretUpgradeMenu::setPosition(const IPoint& position) {
        this->StaticSprite::setPosition(position);
        this->updateRects();
    }

    void TurretUpgradeMenu::setPosition(const DPoint& position) {
        this->StaticSprite::setPosition(position);
        this->updateRects();
    }

    void TurretUpgradeMenu::render() {
        if (!this->shown) return;
        this->StaticSprite::render();
        SDL_RenderCopy(
            this->renderer,
            this->hovered == Sell ? this->sellHovered : this->sell,
            nullptr,
            this->sellRect
        );
        this->sellText->render();
        if (this->canUpgrade) {
            SDL_RenderCopy(
                this->renderer,
                !this->map->getGameState()->canBuy(((TurretData*)this->menuTarget->getTurretData()->upgradePath)->buyPrice)
                ? this->upgradeDisabled
                :
                    this->hovered == Upgrade
                    ? this->upgradeHovered
                    : this->upgrade,
                nullptr,
                this->upgradeRect
            );
            this->upgradeText->render();
        }
    }

    void TurretUpgradeMenu::handleEvent(SDL_Event* event) {
        if (!this->shown) return;
        IPoint position = {
            event->motion.x,
            event->motion.y
        };
        switch (event->type) {
            case SDL_MOUSEMOTION: {
                if (contains(this->sellRect, position)) this->hovered = Sell;
                else if (contains(this->upgradeRect, position)) this->hovered = Upgrade;
                else this->hovered = Neither;
                break;
            };
            case SDL_MOUSEBUTTONUP: {
                if (!contains(this->destRect, position)) {
                    this->closeMenu();
                    return;
                }
                if (this->hovered == Sell) {
                    this->turretHandler->sellTurret(this->menuTarget);
                    this->closeMenu();
                }
                if (
                    this->hovered == Upgrade
                    && this->map->getGameState()->canBuy(((TurretData*)this->menuTarget->getTurretData()->upgradePath)->buyPrice)
                ) {
                    this->turretHandler->upgradeTurret(this->menuTarget);
                    this->closeMenu();
                }
            };
        }
    }

    void TurretUpgradeMenu::loadMenuFor(Turret* target, const IPoint& index) {
        this->menuTarget = target;
        IPoint center = this->map->getTileCenter(index);
        TurretData* data = this->menuTarget->getTurretData();
        this->setDestRect(fromCenter(center, IPoint{160, 160}));
        this->sellText->setText(std::to_string(this->menuTarget->getSellPrice()));
        if (data->upgradePath != nullptr) {
            this->upgradeText->setText(std::to_string(((TurretData*)data->upgradePath)->buyPrice));
            this->canUpgrade = true;
        } else this->canUpgrade = false;
        this->updateRects();
        this->shown = true;
    }

    void TurretUpgradeMenu::closeMenu() {
        this->shown = false;
        this->menuTarget = nullptr;
    }

    bool TurretUpgradeMenu::isShown() {
        return this->shown;
    }
};
