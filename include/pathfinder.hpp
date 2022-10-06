#pragma once
#include <SDL2/SDL.h>
#include "pathfinder_h.hpp"
#include <sprite.hpp>
#include "map_h.hpp"
#include <vector>

namespace game {
    Path::Path(Map* initMap, const std::vector<IPoint>& initPath) : map{initMap}, path{initPath} {}
    Path::~Path() {}

    IPoint Path::next(double scalar, const IPoint& currentPosition, int movementSpeed) {
        if (movementSpeed < 0) throw "Positive movement speed only.";

        IPoint index = this->map->getTileIndex(currentPosition);
        std::vector<IPoint>::iterator i;

        if (index.x == -1 && index.y == -1) {
            std::vector<IPoint>::reverse_iterator r = std::find_if(
                this->path.rbegin(),
                this->path.rend(),
                [&](const IPoint& point) -> bool {
                    return
                        point.x <= index.x + 1
                        && point.x >= index.x - 1
                        && point.y <= index.x + 1
                        && point.y >= index.y - 1;
                }
            );

            if (r == this->path.rend()) throw "Pathfinders must receive a new path when paths are recomputed.";
            i = (++r).base();
        } else {
            i = std::find_if(
                this->path.begin(),
                this->path.end(),
                [&](const IPoint& point) -> bool {
                    return point.x == index.x && point.y == index.y;
                }
            );

            if (i == this->path.end()) throw "Pathfinders must receive a new path when paths are recomputed.";
            if (++i == this->path.end()) return {-1, -1};
        }

        double range = (double)movementSpeed * scalar;
        IPoint targetPosition = this->map->getTileCenter(*i);
        IPoint slope = {
            currentPosition.x - targetPosition.x,
            currentPosition.y - targetPosition.y
        };
        double normalizeScalar = std::sqrt(std::pow((double)slope.x, 2) + std::pow((double)slope.y, 2)) / range;

        slope.x = (int)(slope.x / normalizeScalar);
        slope.y = (int)(slope.y / normalizeScalar);
        return {
            currentPosition.x + slope.x,
            currentPosition.y + slope.y
        };
    }

    Pathfinder::Pathfinder(Map* initMap) : map{initMap} {}
    Pathfinder::~Pathfinder() {}

    const std::vector<Path*>& Pathfinder::pathfind(const std::vector<IPoint>& origins, const IPoint& target) {}
    const std::vector<Path*>& Pathfinder::pathfind(const std::vector<IPoint>& origins, const std::vector<IPoint>& targets) {}
};
