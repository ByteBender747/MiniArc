#pragma once

#include <SDL3/SDL_pixels.h>

namespace sdlc
{

struct RGB {
    int r; // 0-255
    int g; // 0-255
    int b; // 0-255

    RGB(int red = 0, int green = 0, int blue = 0)
        : r(red), g(green), b(blue) {}

    operator SDL_Color() const {
        return {
            static_cast<Uint8>(r),
            static_cast<Uint8>(g),
            static_cast<Uint8>(b),
            255
        };
    }
};

struct HSL {
    double h; // 0-360
    double s; // 0-1
    double l; // 0-1

    HSL(double hue = 0, double saturation = 0, double lightness = 0)
        : h(hue), s(saturation), l(lightness) {}
};

class ColorConverter
{
public:
    static RGB hslToRgb(const HSL& hsl);
    static HSL rgbToHsl(const RGB& rgb);
private:
    static double hueToRgb(double p, double q, double t);
};

} // namespace sdlc