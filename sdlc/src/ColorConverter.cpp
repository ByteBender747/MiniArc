#include <cmath>
#include <algorithm>

#include "MathFunc.hpp"
#include "ColorConverter.hpp"

namespace sdlc
{

RGB ColorConverter::hslToRgb(const HSL& hsl)
{
    double h = clamp(hsl.h, 0.0, 360.0) / 360.0;
    double s = clamp(hsl.s, 0.0, 1.0);
    double l = clamp(hsl.l, 0.0, 1.0);

    double r, g, b;

    if (s == 0.0) {
        // Achromatic (gray)
        r = g = b = l;
    } else {
        double q = l < 0.5 ? l * (1.0 + s) : l + s - l * s;
        double p = 2.0 * l - q;

        r = hueToRgb(p, q, h + 1.0/3.0);
        g = hueToRgb(p, q, h);
        b = hueToRgb(p, q, h - 1.0/3.0);
    }

    return RGB(
               static_cast<int>(std::round(r * 255.0)),
               static_cast<int>(std::round(g * 255.0)),
               static_cast<int>(std::round(b * 255.0))
           );
}

HSL ColorConverter::rgbToHsl(const RGB& rgb)
{
    double r = clamp(rgb.r, 0, 255) / 255.0;
    double g = clamp(rgb.g, 0, 255) / 255.0;
    double b = clamp(rgb.b, 0, 255) / 255.0;

    double max = std::max({r, g, b});
    double min = std::min({r, g, b});
    double delta = max - min;

    double h = 0.0, s = 0.0, l = (max + min) / 2.0;

    if (delta != 0.0) {
        s = l < 0.5 ? delta / (max + min) : delta / (2.0 - max - min);

        if (max == r) {
            h = ((g - b) / delta) + (g < b ? 6.0 : 0.0);
        } else if (max == g) {
            h = ((b - r) / delta) + 2.0;
        } else {
            h = ((r - g) / delta) + 4.0;
        }

        h /= 6.0;
    }

    return HSL(h * 360.0, s, l);
}

double ColorConverter::hueToRgb(double p, double q, double t)
{
    if (t < 0.0) t += 1.0;
    if (t > 1.0) t -= 1.0;
    if (t < 1.0/6.0) return p + (q - p) * 6.0 * t;
    if (t < 1.0/2.0) return q;
    if (t < 2.0/3.0) return p + (q - p) * (2.0/3.0 - t) * 6.0;
    return p;
}

} // namespace sdlc