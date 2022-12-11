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

        const IPoint
            n = {0, -1},
            ne = {1, -1},
            e = {1, 0},
            se = {1, 1},
            s = {0, 1},
            sw = {-1, 1},
            w = {-1, 0},
            nw = {-1, -1};

        class Neighborhood {
        public:
            IPoint
                actual,
                origin,
                current,
                target;

            Neighborhood(
                const IPoint& initActual,
                const IPoint& initOrigin,
                const IPoint& initCurrent,
                const IPoint& initTarget
            );
        };

        virtual Neighborhood getNeighborhood(const DPoint& currentPosition);
    public:
        Path(Map* initMap, const std::vector<IPoint>& initPath);
        ~Path();

        virtual bool isIndexInPath(const IPoint& index);

        virtual DPoint next(double scalar, const DPoint& currentPosition, int movementSpeed);
    };
};
