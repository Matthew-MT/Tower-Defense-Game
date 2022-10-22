#pragma once
#include <SDL2/SDL.h>
#include "pathfinder_h.hpp"
#include "sprite.hpp"
#include "map_h.hpp"
#include <vector>

namespace game {
    Path::Path(Map* initMap, const std::vector<IPoint>& initPath) : map{initMap}, path{initPath} {
        std::string message = "Path:";
        for (const IPoint& index : this->path) message += " (" + std::to_string(index.x) + ", " + std::to_string(index.y) + ")";
        SDL_Log(message.c_str());
    }
    Path::~Path() {}

    bool Path::isIndexInPath(const IPoint& index) {
        std::string message = "Path:";
        for (const IPoint& index : this->path) message += " (" + std::to_string(index.x) + ", " + std::to_string(index.y) + ")";
        SDL_Log(message.c_str());
        SDL_Log(("Index: (" + std::to_string(index.x) + ", " + std::to_string(index.y) + ")").c_str());
        return std::find(this->path.begin(), this->path.end(), index) != this->path.end();
    }

    DPoint Path::next(double scalar, const DPoint& currentPosition, int movementSpeed) {        
        if (movementSpeed < 0) throw "Positive movement speed only.";

        IPoint index = this->map->getTileIndex({
            (int)currentPosition.x,
            (int)currentPosition.y
        });
        std::vector<IPoint>::iterator i = std::find(this->path.begin(), this->path.end(), index);
        DPoint targetPosition = currentPosition;

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

            if (r == this->path.rend()) throw "Pathfinders must receive a new path when paths are recomputed.";
            i = (++r).base();
            targetPosition = this->map->getTileCenter(*i);
        } else {
            // std::cout << "Origin: (" << (*i).x << ", " << (*i).y << ")\n";
            // SDL_Log(("Origin: (" + std::to_string((*i).x) + ", " + std::to_string((*i).y) + ")").c_str());
            // std::cout << "Considering: (" << (*(i + 1)).x << ", " << (*(i + 1)).y << ")\n";
            // SDL_Log(("Considering: (" + std::to_string((*(i + 1)).x) + ", " + std::to_string((*(i + 1)).y) + ")").c_str());
            targetPosition = this->map->getTileCenter(*i);
            if (i + 1 != this->path.end()) {
                DPoint consideredPosition = this->map->getTileCenter(*(i + 1));
                if (
                    distance(currentPosition, targetPosition) <= range
                    || distance(currentPosition, consideredPosition) <= distance(consideredPosition, targetPosition)
                ) targetPosition = consideredPosition;
            } else return {std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};
        }
        DPoint slope = {
            (double)targetPosition.x - currentPosition.x,
            (double)targetPosition.y - currentPosition.y
        };

        double normalizeScalar = std::sqrt(std::pow((double)slope.x, 2) + std::pow((double)slope.y, 2)) / range;

        // std::cout << "Target: (" << targetPosition.x << ", " << targetPosition.y << ")\n";
        // SDL_Log(("Target: (" + std::to_string(targetPosition.x) + ", " + std::to_string(targetPosition.y) + ")").c_str());
        // std::cout << "Normal: " << normalizeScalar << "\n";
        // SDL_Log(("Normal: " + std::to_string(normalizeScalar)).c_str());

        slope.x = slope.x / normalizeScalar;
        slope.y = slope.y / normalizeScalar;
        // std::cout << "Slope: (" << slope.x << ", " << slope.y << ")\n";
        // SDL_Log(("Slope: (" + std::to_string(slope.x) + ", " + std::to_string(slope.y) + ")").c_str());
        return {
            currentPosition.x + slope.x,
            currentPosition.y + slope.y
        };
    }
};
