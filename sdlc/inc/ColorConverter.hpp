#pragma once

#include <SDL3/SDL_pixels.h>
#include <cstdint>

namespace sdlc
{

struct RGB {
    uint8_t r; // 0-255
    uint8_t g; // 0-255
    uint8_t b; // 0-255

    RGB(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0)
        : r(red), g(green), b(blue) {}

    operator SDL_Color() const {
        return {
            static_cast<Uint8>(r),
            static_cast<Uint8>(g),
            static_cast<Uint8>(b),
            255
        };
    }

    bool operator==(const RGB& other) const {
        return r == other.r && g == other.g && b == other.b;
    }
};

struct RGBA {
    uint8_t r; // 0-255
    uint8_t g; // 0-255
    uint8_t b; // 0-255
    uint8_t a; // 0-255

    RGBA(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0, uint8_t alpha = 255)
        : r(red), g(green), b(blue), a(alpha) {}

    // Constructor from RGB (with optional alpha)
    RGBA(const RGB& rgb, uint8_t alpha = 255)
        : r(rgb.r), g(rgb.g), b(rgb.b), a(alpha) {}

    operator SDL_Color() const {
        return {
            static_cast<Uint8>(r),
            static_cast<Uint8>(g),
            static_cast<Uint8>(b),
            static_cast<Uint8>(a)
        };
    }

    bool operator==(const RGBA& other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }
};

struct HSL {
    double h; // 0-360
    double s; // 0-1
    double l; // 0-1

    HSL(double hue = 0, double saturation = 0, double lightness = 0)
        : h(hue), s(saturation), l(lightness) {}
};

RGB hslToRgb(const HSL& hsl);
HSL rgbToHsl(const RGB& rgb);

} // namespace sdlc