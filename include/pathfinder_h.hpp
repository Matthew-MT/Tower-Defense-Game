#pragma once
#include <SDL2/SDL.h>
#include "forward.hpp"
#include "sprite.hpp"
#include <vector>

namespace game {
    class Path {
    protected:
        Map* map;
        std::vector<IPoint> path;
    public:
        Path(Map* initMap, const std::vector<IPoint>& initPath);
        ~Path();

        IPoint next(double scalar, const IPoint& currentPosition, int movementSpeed);
    };

    class Pathfinder {
    protected:
        Map* map;
    public:
        Pathfinder(Map* initMap);
        ~Pathfinder();

        const std::vector<Path*>& pathfind(Node* origin, const std::vector<Node*>& targets);
        const std::vector<Path*>& pathfind(const IPoint& origin, const std::vector<IPoint>& targets);
        const std::vector<Path*>& pathfind(const std::vector<IPoint>& origins, const std::vector<IPoint>& targets);
    };
};
