#pragma once

#define __VECTOR2_INCLUDED__

#include <cmath>

#include "MathFunc.hpp"

namespace sdl {

template <typename ValueT>
struct Vec2 {
    ValueT x, y;
    Vec2()
        : x(0)
        , y(0) {
    }
    Vec2(ValueT xy)
        : x(xy)
        , y(xy) {
    }
    Vec2(ValueT x, ValueT y)
        : x(x)
        , y(y) {
    }
    Vec2& operator+=(const Vec2& rhs) {
        _vadd<ValueT, 2>(reinterpret_cast<ValueT*>(this), reinterpret_cast<const ValueT*>(this),
                         reinterpret_cast<const ValueT*>(&rhs));
        return *this;
    }
    Vec2& operator-=(const Vec2& rhs) {
        _vsub<ValueT, 2>(reinterpret_cast<ValueT*>(this), reinterpret_cast<const ValueT*>(this),
                         reinterpret_cast<const ValueT*>(&rhs));
        return *this;
    }
    Vec2& operator*=(const Vec2& rhs) {
        _vmul<ValueT, 2>(reinterpret_cast<ValueT*>(this), reinterpret_cast<const ValueT*>(this),
                         reinterpret_cast<const ValueT*>(&rhs));
        return *this;
    }
    Vec2& operator/=(const Vec2& rhs) {
        _vdiv<ValueT, 2>(reinterpret_cast<ValueT*>(this), reinterpret_cast<const ValueT*>(this),
                         reinterpret_cast<const ValueT*>(&rhs));
        return *this;
    }
    template <typename pT>
    pT get() {
        pT result;
        result.x = x;
        result.y = y;
        return result;
    }
    template <typename pT>
    void put(pT& ref) {
        ref.x = x;
        ref.y = y;
    }
    static Vec2 up() {
        return Vec2(0, -1);
    }
    static Vec2 down() {
        return Vec2(0, 1);
    }
    static Vec2 left() {
        return Vec2(-1, 0);
    }
    static Vec2 right() {
        return Vec2(1, 0);
    }
};

using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;
using Vec2i = Vec2<int>;

template <typename ValueT>
inline Vec2<ValueT> operator+(Vec2<ValueT> lhs, const Vec2<ValueT>& rhs)
{
    lhs += rhs;
    return lhs;
}

template <typename ValueT>
inline Vec2<ValueT> operator-(Vec2<ValueT> lhs, const Vec2<ValueT>& rhs)
{
    lhs -= rhs;
    return lhs;
}

template <typename ValueT>
inline Vec2<ValueT> operator*(Vec2<ValueT> lhs, const Vec2<ValueT>& rhs)
{
    lhs *= rhs;
    return lhs;
}

template <typename ValueT>
inline Vec2<ValueT> operator/(Vec2<ValueT> lhs, const Vec2<ValueT>& rhs)
{
    lhs /= rhs;
    return lhs;
}

template <typename ValueT>
inline float dot(const Vec2<ValueT>& a, const Vec2<ValueT>& b)
{
    Vec2<ValueT> ab = a * b;
    return ab.x + ab.y;
}

template <typename ValueT>
inline double length(const Vec2<ValueT>& a)
{
    return sqrt(dot(a, a));
}

template <typename ValueT>
inline double lengthSquared(const Vec2<ValueT>& a)
{
    return dot(a, a);
}

template <typename ValueT>
inline float distance(const Vec2<ValueT>& a, const Vec2<ValueT>& b)
{
    Vec2<ValueT> ab = a - b;
    return length(ab);
}

template <typename ValueT>
inline float squareDistance(const Vec2<ValueT>& a, const Vec2<ValueT>& b)
{
    Vec2<ValueT> ab = a - b;
    return dot(ab, ab);
}

template <typename ValueT>
inline Vec2<ValueT> direction(const Vec2<ValueT>& a, const Vec2<ValueT>& b)
{
    Vec2<ValueT> ab = a - b;
    return normalized(ab);
}

template <typename ValueT>
inline Vec2<ValueT> normalized(const Vec2<ValueT>& vec)
{
    float len = length(vec);
    return Vec2<ValueT>(vec.x / len, vec.y / len);
}

template <typename ValueT>
inline Vec2<ValueT> rotated(const Vec2<ValueT>& origin, double angle, ValueT radius)
{
    Vec2<ValueT> rot;
    rot.x = origin.x + cos(angle) * radius;
    rot.y = origin.y + sin(angle) * radius;
    return rot;
}

template <typename ValueT>
inline Vec2<ValueT> reflect(const Vec2<ValueT>& d, const Vec2<ValueT>& n)
{
    ValueT dn = 2 * dot(d, n);
    return d - n * Vec2<ValueT>(dn);
}

template <typename ValueT>
inline bool withinRange(const Vec2<ValueT>& pos, const Vec2<ValueT>& target, ValueT range)
{
    Vec2<ValueT> dir = target - pos;
    return dot(dir, dir) < (range * range);
}
template <typename ValueT>
inline bool outerRange(const Vec2<ValueT>& pos, const Vec2<ValueT>& target, ValueT range)
{
    Vec2<ValueT> dir = target - pos;
    return dot(dir, dir) > (range * range);
}

template <typename ValueT>
inline double angleBetween(const Vec2<ValueT>& a, const Vec2<ValueT>& b)
{
    Vec2<ValueT> d = a - b;
    return atan2(d.y, d.x);
}

template <typename ValueT>
inline double angleTo(const Vec2<ValueT>& d)
{
    return atan2(d.y, d.x);
}

} // namespace sdl
