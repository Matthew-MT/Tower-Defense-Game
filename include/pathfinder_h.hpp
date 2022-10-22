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

        virtual bool isIndexInPath(const IPoint& index);

        virtual DPoint next(double scalar, const DPoint& currentPosition, int movementSpeed);
    };
};
