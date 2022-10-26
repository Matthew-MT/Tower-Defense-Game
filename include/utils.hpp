#pragma once
#include <SDL2/SDL.h>
#include <cmath>

namespace game {
    template<typename Type> class Point {
    public:
        Type x, y;
        template<typename Other> Point(
            const Point<Other>& other
        ) :
            x{(Type)other.x},
            y{(Type)other.y} {}
        Point(Type ix, Type iy) : x{ix}, y{iy} {}

        bool operator == (const Point<Type>& other) const {
            return this->x == other.x && this->y == other.y;
        }

        bool operator != (const Point<Type>& other) const {
            return this->x != other.x || this->y != other.y;
        }
    };

    using IPoint = Point<int>;
    using DPoint = Point<double>;

    double distance(DPoint a, DPoint b) {
        return std::sqrt(
            std::pow(a.x - b.x, 2)
            + std::pow(a.y - b.y, 2)
        );
    }

    double distance(IPoint a, IPoint b) {
        return distance((DPoint)a, (DPoint)b);
    }

    double distanceCenter(SDL_Rect* a, SDL_Rect* b) {
        return distance(
            IPoint{
                a->x + (a->w >> 1),
                a->y + (a->h >> 1)
            },
            IPoint{
                b->x + (b->w >> 1),
                b->y + (b->h >> 1)
            }
        );
    }

    const double sqrtOf2 = distance(IPoint{0, 0}, IPoint{1, 1});

    template<typename Position, typename Size> SDL_Rect* createRect(const Point<Position>& position, const Point<Size>& size) {
        SDL_Rect* rect = new SDL_Rect();
        rect->x = (int)position.x;
        rect->y = (int)position.y;
        rect->w = (int)size.x;
        rect->h = (int)size.y;
        return rect;
    }
};
