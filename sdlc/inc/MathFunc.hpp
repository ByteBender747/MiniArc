#pragma once

#include <cstdlib>

namespace sdlc {

constexpr double pi = 3.14159265358979323846;
constexpr double tau = pi * 2.;

constexpr double deg2rad(double x)
{
    return x / 360. * tau;
}

constexpr double rad2deg(double x)
{
    return x * 360. / tau;
}

template <typename ValueT>
static inline ValueT lerp(const ValueT& min, const ValueT& max, double time)
{
    return min + (max - min) * time;
}

template <typename ValueT>
static inline ValueT clamp(ValueT v, ValueT min, ValueT max)
{
    return (v < min) ? min : (v > max) ? max : v;
}

template <typename ValueT>
static inline ValueT saturate(ValueT v)
{
    return clamp<ValueT>(v, 0, 1);
}

template <typename ValueT, std::size_t n>
inline void _vadd(ValueT c[], const ValueT a[], const ValueT b[])
{
    for (std::size_t i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

template <typename ValueT, std::size_t n>
inline void _vsub(ValueT c[], const ValueT a[], const ValueT b[])
{
    for (std::size_t i = 0; i < n; ++i) {
        c[i] = a[i] - b[i];
    }
}

template <typename ValueT, std::size_t n>
inline void _vmul(ValueT c[], const ValueT a[], const ValueT b[])
{
    for (std::size_t i = 0; i < n; ++i) {
        c[i] = a[i] * b[i];
    }
}

template <typename ValueT, std::size_t n>
inline void _vdiv(ValueT c[], const ValueT a[], const ValueT b[])
{
    for (std::size_t i = 0; i < n; ++i) {
        c[i] = a[i] / b[i];
    }
}

} // namespace sdlc
