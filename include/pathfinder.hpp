#pragma once
#include <SDL2/SDL.h>
#include "pathfinder_h.hpp"
#include "sprite.hpp"
#include "map_h.hpp"
#include <vector>

namespace game {
    Path::Neighborhood::Neighborhood(
        const IPoint& initActual,
        const IPoint& initOrigin,
        const IPoint& initCurrent,
        const IPoint& initTarget
    ) :
        actual{initActual},
        origin{initOrigin},
        current{initCurrent},
        target{initTarget} {}

    Path::Neighborhood Path::getNeighborhood(const DPoint& currentPosition) {
        IPoint actual = this->map->getTileIndex(currentPosition);

        std::vector<IPoint>::iterator current = std::find(this->path.begin(), this->path.end(), actual);
        if (current == this->path.end()) {
            std::vector<IPoint>::reverse_iterator r = std::find_if(
                this->path.rbegin(),
                this->path.rend(),
                [&](const IPoint& point) -> bool {
                    return
                        point.x <= actual.x + 1
                        && point.x >= actual.x - 1
                        && point.y <= actual.y + 1
                        && point.y >= actual.y - 1;
                }
            );

            if (r == this->path.rend()) {
                SDL_Log("Pathfinders must receive a new path when paths are recomputed.");
                throw 1;
            }

            current = (++r).base();
        }

        std::vector<IPoint>::iterator origin = current, target = current;
        if (origin != this->path.begin()) origin--;
        if (target + 1 != this->path.end()) target++;

        return {
            actual,
            *origin,
            *current,
            *target
        };
    }

    Path::Path(Map* initMap, const std::vector<IPoint>& initPath) : map{initMap}, path{initPath} {}
    Path::~Path() {}

    bool Path::isIndexInPath(const IPoint& index) {
        return std::find(this->path.begin(), this->path.end(), index) != this->path.end();
    }

    DPoint Path::next(double scalar, const DPoint& currentPosition, int movementSpeed) {
        if (movementSpeed < 0) throw "Positive movement speed only.";

        IPoint index = this->map->getTileIndex(currentPosition);
        std::vector<IPoint>::iterator i = std::find(this->path.begin(), this->path.end(), index);
        DPoint
            originPosition = currentPosition,
            targetPosition = currentPosition;

        double range = (double)movementSpeed * scalar;

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

            if (r == this->path.rend()) {
                SDL_Log("Pathfinders must receive a new path when paths are recomputed.");
                throw 1;
            }
            i = (++r).base();
            targetPosition = this->map->getTileCenter(*i);
            // if (i != this->path.begin()) originPosition = this->map->getTileCenter(*(i - 1));
        } else {
            targetPosition = this->map->getTileCenter(*i);
            if (i + 1 != this->path.end()) {
                DPoint consideredPosition = this->map->getTileCenter(*(i + 1));
                if (
                    distance(currentPosition, targetPosition) <= range
                    || distance(currentPosition, consideredPosition) <= distance(consideredPosition, targetPosition)
                ) {
                    originPosition = targetPosition;
                    targetPosition = consideredPosition;
                } else if (i != this->path.begin()) originPosition = this->map->getTileCenter(*(i - 1));
            } else return {std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};
        }

        // SDL_Log((
        //     "\nCurrent: (" + std::to_string(currentPosition.x) + ", " + std::to_string(currentPosition.y) + ")\n"
        //     + "Origin:  (" + std::to_string(originPosition.x) + ", " + std::to_string(originPosition.y) + ")\n"
        //     + "Target:  (" + std::to_string(targetPosition.x) + ", " + std::to_string(targetPosition.y) + ")"
        // ).c_str());

        DPoint slope = targetPosition - currentPosition;

        // if (originPosition == targetPosition) slope -= currentPosition;
        // else slope -= originPosition;

        // double normalizeScalar = std::sqrt(std::pow((double)slope.x, 2) + std::pow((double)slope.y, 2)) / range;

        return currentPosition + slope.normal(range);
    }

    // DPoint Path::next(double scalar, const DPoint& currentPosition, int movementSpeed) {
    //     const Path::Neighborhood nh = this->getNeighborhood(currentPosition);
    //     IPoint size = this->map->getTileSize();

    //     if (nh.origin == nh.current + this->n) {
    //         if (nh.target == nh.current + this->e) {
    //             IPoint corner = this->map->getTileCenter(nh.current);
    //             corner.x += size.x >> 1;
    //             corner.y -= size.y >> 1;

    //             if (distance(currentPosition, (DPoint)corner) >= size.x)
    //         }
    //     }
    // }
};
