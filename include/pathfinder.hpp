#pragma once
#include <SDL2/SDL.h>
#include "pathfinder_h.hpp"
#include "sprite.hpp"
#include "map_h.hpp"
#include <vector>

namespace game {
    Path::Path(Map* initMap, const std::vector<IPoint>& initPath) : map{initMap}, path{initPath} {}
    Path::~Path() {}

    bool Path::isIndexInPath(const IPoint& index) {
        return std::find(this->path.begin(), this->path.end(), index) != this->path.end();
    }

    IPoint Path::next(double scalar, const IPoint& currentPosition, int movementSpeed) {        
        if (movementSpeed < 0) throw "Positive movement speed only.";

        IPoint index = this->map->getTileIndex(currentPosition);
        std::vector<IPoint>::iterator i = std::find(this->path.begin(), this->path.end(), index);

        if (i == this->path.end()) {
            std::vector<IPoint>::reverse_iterator r = std::find_if(
                this->path.rbegin(),
                this->path.rend(),
                [&](const IPoint& point) -> bool {
                    return
                        point.x <= index.x + 1
                        && point.x >= index.x - 1
                        && point.y <= index.y + 1
                        && point.y >= index.y - 1;
                }
            );

            if (r == this->path.rend()) throw "Pathfinders must receive a new path when paths are recomputed.";
            i = (++r).base();
        } else if (++i == this->path.end()) return {-1, -1};

        double range = (double)movementSpeed * scalar;
        IPoint targetPosition = this->map->getTileCenter(*i);
        IPoint slope = {
            targetPosition.x - currentPosition.x,
            targetPosition.y - currentPosition.y
        };

        double normalizeScalar = std::sqrt(std::pow((double)slope.x, 2) + std::pow((double)slope.y, 2)) / range;

        SDL_Log(("Normal: " + std::to_string(normalizeScalar)).c_str());

        slope.x = (int)(slope.x / normalizeScalar);
        slope.y = (int)(slope.y / normalizeScalar);
        return {
            currentPosition.x + slope.x,
            currentPosition.y + slope.y
        };
    }
};
