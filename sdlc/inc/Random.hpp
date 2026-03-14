#pragma once

#include <cstdint>
#include <SDL3/SDL_timer.h>

namespace sdlc
{

class Random
{
public:
    Random() {
        m_s[0] = xorshift64(c_x64_init ^ SDL_GetPerformanceCounter());
        m_s[1] = xorshift64(m_s[0]);
    }
    Random(uint64_t seed) {
        uint64_t i = c_x64_init ^ seed;
        m_s[0] = xorshift64(i ? i : c_x64_init);
        m_s[1] = xorshift64(m_s[0]);
    }
    static uint64_t xorshift64(uint64_t x64) {
        x64 ^= x64 << 13;
        x64 ^= x64 >> 7;
        x64 ^= x64 << 17;
        return x64;
    }
    uint64_t next() noexcept {
        uint64_t x = m_s[0];
        uint64_t y = m_s[1];
        m_s[0] = y;
        x ^= x << 23;
        m_s[1] = x ^ y ^ (x >> 17) ^ (y >> 26);
        return m_s[1] + y;
    }
    template <typename ValueT>
    ValueT next() {
        return static_cast<ValueT>(next());
    }
    bool chance(double probability) noexcept {
        if (probability <= 0.)
            return false;
        else if (probability >= 1.)
            return true;
        else
            return (static_cast<double>(UINT64_MAX) * probability) >= next();
    }
    double uniform() noexcept {
        return next() / static_cast<double>(UINT64_MAX);
    }
    double normal() noexcept {
        return next<int64_t>() / static_cast<double>(INT64_MAX);
    }

private:
    const uint64_t c_x64_init = 8446908529444109602ull;
    uint64_t m_s[2];
};

} // namespace sdlc
