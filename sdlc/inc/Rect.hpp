#pragma once

#include <SDL3/SDL_rect.h>

namespace sdlc
{

template <typename valueT>
struct Point2D {
    valueT x{0}, y{0};
    Point2D() = default;
    Point2D(valueT x, valueT y) : x(x), y(y) {}
    Point2D(const SDL_FPoint& r) {
        x = static_cast<valueT>(r.x);
        y = static_cast<valueT>(r.y);
    }
    Point2D(const SDL_Point& r) {
        x = static_cast<valueT>(r.x);
        y = static_cast<valueT>(r.y);
    }
    operator SDL_Point() const {
        return {
            static_cast<int>(x),
            static_cast<int>(y)
        };
    }

    operator SDL_FPoint() const {
        return {
            static_cast<float>(x),
            static_cast<float>(y)
        };
    }
};

template <typename valueT>
struct Rect {
    Point2D<valueT> t;
    Point2D<valueT> b;

    Rect() = default;
    Rect(valueT x, valueT y, valueT w, valueT h) {
        t.x = x;
        t.y = y;
        b.x = x + w;
        b.y = y + h;
    }

    Rect(const SDL_FRect& r) {
        t.x = static_cast<valueT>(r.x);
        t.y = static_cast<valueT>(r.y);
        b.x = static_cast<valueT>(r.x) + static_cast<valueT>(r.w);
        b.y = static_cast<valueT>(r.y) + static_cast<valueT>(r.h);
    }

    Rect(const SDL_Rect& r) {
        t.x = static_cast<valueT>(r.x);
        t.y = static_cast<valueT>(r.y);
        b.x = static_cast<valueT>(r.x) + static_cast<valueT>(r.w);
        b.y = static_cast<valueT>(r.y) + static_cast<valueT>(r.h);
    }

    valueT width() const {
        return b.x - t.x;
    }

    valueT height() const {
        return b.y - t.y;
    }

    bool contains(const Point2D<valueT>& p) const {
        return p.x > t.x && p.y > t.y && p.x < b.x && p.y < b.y;
    }

    bool intersects(const Rect& other) const {
        return t.x < other.b.x && b.x > other.t.x && t.y < other.b.y && b.y > other.t.y;
    }

    Rect moved(valueT x, valueT y) const {
        Rect r;
        r.t.x = t.x + x;
        r.t.y = t.y + y;
        r.b.x = b.x + x;
        r.b.y = b.y + y;
        return r;
    }

    Rect offset(valueT x, valueT y) const {
        Rect r;
        r.t.x = t.x + x;
        r.t.y = t.y + y;
        r.b.x = b.x - x;
        r.b.y = b.y - y;
        return r;
    }

    operator SDL_Rect() const {
        return {
            static_cast<int>(t.x),
            static_cast<int>(t.y),
            static_cast<int>(width()),
            static_cast<int>(height())
        };
    }

    operator SDL_FRect() const {
        return {
            static_cast<float>(t.x),
            static_cast<float>(t.y),
            static_cast<float>(width()),
            static_cast<float>(height())
        };
    }
};

} // namespace sdlc