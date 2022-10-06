#pragma once

namespace game {
    template<typename Type> class Point {
    public:
        Type x, y;
        Point(Type ix, Type iy) : x{ix}, y{iy} {}
    };

    using IPoint = Point<int>;
    double distance(IPoint a, IPoint b) {
        return std::sqrt(
            std::pow(a.x - b.x, 2)
            + std::pow(a.y - b.y, 2)
        );
    }

    double distanceCenter(SDL_Rect* a, SDL_Rect* b) {
        return distance(
            {
                a->x + (a->w >> 1),
                a->y + (a->h >> 1)
            },
            {
                b->x + (b->w >> 1),
                b->y + (b->h >> 1)
            }
        );
    }
};
