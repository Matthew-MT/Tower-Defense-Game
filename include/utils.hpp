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

        template<typename Other> Point<Type> operator + (const Point<Other>& other) {
            return {
                this->x + (Type)other.x,
                this->y + (Type)other.y
            };
        }

        template<typename Other> Point<Type> operator - (const Point<Other>& other) {
            return {
                this->x - (Type)other.x,
                this->y - (Type)other.y
            };
        }
    };

    using IPoint = Point<int>;
    using DPoint = Point<double>;

    IPoint getCenter(SDL_Rect* rect) {
        return {
            rect->x + (rect->w >> 1),
            rect->y + (rect->h >> 1)
        };
    }

    template<typename Position, typename Size> SDL_Rect* fromCenter(const Point<Position>& position, const Point<Size>& size) {
        SDL_Log(("Position:   (" + std::to_string(position.x) + ", " + std::to_string(position.y) + "); (" + std::to_string(size.x) + ", " + std::to_string(size.y) + ")").c_str());
        SDL_Rect* rect = new SDL_Rect();
        rect->x = (int)position.x - (int)(size.x >> 1);
        rect->y = (int)position.y - (int)(size.y >> 1);
        rect->w = size.x;
        rect->h = size.y;
        SDL_Log(("Calculated: (" + std::to_string(rect->x) + ", " + std::to_string(rect->y) + "); (" + std::to_string(rect->w) + ", " + std::to_string(rect->h) + ")").c_str());
        return rect;
    }

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
