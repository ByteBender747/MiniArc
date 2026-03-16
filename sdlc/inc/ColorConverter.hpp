#pragma once

#include <cstdint>
#include <SDL3/SDL_pixels.h>

namespace sdlc
{

// hue color offsets
constexpr double hueRed       = 0;
constexpr double hueYellow    = 60;
constexpr double hueOrange    = 26;
constexpr double hueLime      = 80;
constexpr double hueGreen     = 120;
constexpr double hueCyan      = 180;
constexpr double hueLightBlue = 200;
constexpr double hueBlue      = 240;
constexpr double huePurple    = 320;
constexpr double hueMagenta   = 300;

struct RGB {
    uint8_t r; // 0-255
    uint8_t g; // 0-255
    uint8_t b; // 0-255

    RGB(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0)
        : r(red), g(green), b(blue) {}

    RGB(const SDL_Color& ref)
        : r(ref.r), g(ref.g), b(ref.b) {}

    RGB(const SDL_FColor& ref)
        : r(ref.r * 255), g(ref.g * 255), b(ref.b * 255) {}

    operator SDL_Color() const {
        return {
            static_cast<Uint8>(r),
            static_cast<Uint8>(g),
            static_cast<Uint8>(b),
            255
        };
    }

    operator SDL_FColor() const {
        return {
            static_cast<float>(r) / 255.0f,
            static_cast<float>(g) / 255.0f,
            static_cast<float>(b) / 255.0f,
            1.0f
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

    RGBA(const SDL_Color& ref)
        : r(ref.r), g(ref.g), b(ref.b), a(ref.a) {}

    RGBA(const SDL_FColor& ref)
        : r(ref.r * 255), g(ref.g * 255), b(ref.b * 255), a(ref.a * 255) {}

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

    operator SDL_FColor() const {
        return {
            static_cast<float>(r) / 255.0f,
            static_cast<float>(g) / 255.0f,
            static_cast<float>(b) / 255.0f,
            static_cast<float>(a) / 255.0f
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