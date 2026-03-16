#include <cstdint>
#include <stack>
#include <cstdlib>
#include <utility>
#include <cassert>
#include <cmath>

#include <SDL3/SDL_pixels.h>

#include "Painter.hpp"
#include "Dimension.hpp"
#include "Rect.hpp"

namespace sdlc
{

SurfacePainter::SurfacePainter(SDL_Surface* surface)
    : m_surface(surface)
{
    assert(m_surface);
    setDimension(m_surface->w, m_surface->h);
    const SDL_PixelFormatDetails* fmt = SDL_GetPixelFormatDetails(m_surface->format);
    memcpy(&m_pixelFormat, fmt, sizeof(SDL_PixelFormatDetails));
}

void SurfacePainter::setPixel(int x, int y)
{
    if (x < 0 || x >= m_surface->w || y < 0 || y >= m_surface->h) {
        return;
    }

    uint32_t pixelValue;
    uint32_t offset = m_surface->pitch * y + x * m_pixelFormat.bytes_per_pixel;
    uint8_t* pixelData = static_cast<uint8_t*>(m_surface->pixels);
    if (m_pixelFormat.Abits > 0) {
        pixelValue = SDL_MapRGBA(&m_pixelFormat, nullptr, color().r, color().g, color().b, color().a);
    } else {
        pixelValue = SDL_MapRGB(&m_pixelFormat, nullptr, color().r, color().g, color().b);
    }

    // TODO: check for possible endianness issue
    memcpy(&pixelData[offset], &pixelValue, m_pixelFormat.bytes_per_pixel);
}

RGBA SurfacePainter::getPixel(int x, int y)
{
    if (x < 0 || x >= m_surface->w || y < 0 || y >= m_surface->h) {
        return {0, 0, 0, 0};
    }

    uint32_t pixelValue;
    uint32_t offset = m_surface->pitch * y + x * m_pixelFormat.bytes_per_pixel;
    uint8_t* pixelData = static_cast<uint8_t*>(m_surface->pixels);

    // TODO: check for possible endianness issue
    memcpy(&pixelValue, &pixelData[offset], m_pixelFormat.bytes_per_pixel);

    RGBA result{0, 0, 0, 255};
    if (m_pixelFormat.Abits > 0) {
        SDL_GetRGBA(pixelValue, &m_pixelFormat, nullptr, &result.r, &result.g, &result.b, &result.a);
    } else {
        SDL_GetRGB(pixelValue, &m_pixelFormat, nullptr, &result.r, &result.g, &result.b);
    }
    return result;
}

void Painter::moveTo(int x, int y)
{
    m_cursor = Point2D<int>(x, y);
}

void Painter::lineTo(int x, int y)
{
    line(m_cursor.x, m_cursor.y, x, y);
    moveTo(x, y);
}

void Painter::line(int x0, int y0, int x1, int y1)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */

    for (;;) { /* loop */
        setPixel(x0, y0);
        if (x0 == x1 && y0 == y1)
            break;
        e2 = 2 * err;
        if (e2 > dy) {
            err += dy;
            x0 += sx;
        } /* e_xy+e_x > 0 */
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        } /* e_xy+e_y < 0 */
    }
}

void Painter::circle(int x0, int y0, int radius)
{
    int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

    setPixel(x0, y0 + radius);
    setPixel(x0, y0 - radius);
    setPixel(x0 + radius, y0);
    setPixel(x0 - radius, y0);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x + 1;

        setPixel(x0 + x, y0 + y);
        setPixel(x0 - x, y0 + y);
        setPixel(x0 + x, y0 - y);
        setPixel(x0 - x, y0 - y);
        setPixel(x0 + y, y0 + x);
        setPixel(x0 - y, y0 + x);
        setPixel(x0 + y, y0 - x);
        setPixel(x0 - y, y0 - x);
    }
}

void Painter::ellipse(int xm, int ym, int a, int b)
{
    int dx = 0, dy = b; /* im I. Quadranten von links oben nach rechts unten */
    long a2 = a * a, b2 = b * b;
    long err = b2 - (2 * b - 1) * a2, e2; /* Fehler im 1. Schritt */

    do {
        setPixel(xm + dx, ym + dy); /* I. Quadrant */
        setPixel(xm - dx, ym + dy); /* II. Quadrant */
        setPixel(xm - dx, ym - dy); /* III. Quadrant */
        setPixel(xm + dx, ym - dy); /* IV. Quadrant */

        e2 = 2 * err;
        if (e2 < (2 * dx + 1) * b2) {
            dx++;
            err += (2 * dx + 1) * b2;
        }
        if (e2 > -(2 * dy - 1) * a2) {
            dy--;
            err -= (2 * dy - 1) * a2;
        }
    } while (dy >= 0);

    while (dx++ < a) { /* fehlerhafter Abbruch bei flachen Ellipsen (b=1) */
        setPixel(xm + dx, ym); /* -> Spitze der Ellipse vollenden */
        setPixel(xm - dx, ym);
    }
}

void Painter::arc(int x0, int y0, int radius, double startAngle, double endAngle)
{
    // Convert angles from degrees to radians
    const double PI = 3.14159265358979323846;
    double start = startAngle * PI / 180.0;
    double end = endAngle * PI / 180.0;

    // Normalize angles to [0, 2*PI)
    while (start < 0) start += 2 * PI;
    while (end < 0) end += 2 * PI;
    while (start >= 2 * PI) start -= 2 * PI;
    while (end >= 2 * PI) end -= 2 * PI;

    // Handle wraparound case
    if (end < start) {
        end += 2 * PI;
    }

    // Calculate step size based on radius for smooth arc
    // Smaller radius = fewer steps needed
    double step = 1.0 / (radius > 0 ? radius : 1);

    // Draw the arc
    for (double angle = start; angle <= end; angle += step) {
        int x = static_cast<int>(x0 + radius * cos(angle) + 0.5);
        int y = static_cast<int>(y0 + radius * sin(angle) + 0.5);
        setPixel(x, y);
    }

    // Ensure the end point is drawn
    int x_end = static_cast<int>(x0 + radius * cos(end) + 0.5);
    int y_end = static_cast<int>(y0 + radius * sin(end) + 0.5);
    setPixel(x_end, y_end);
    moveTo(x_end, y_end);
}

void Painter::floodFill(int x, int y)
{
    RGBA oldColor = getPixel(x, y);
    RGBA newColor = color();

    if (oldColor == newColor) {
        return; // No need to fill if colors are already the same
    }

    std::stack<std::pair<int, int>> pixelStack;
    pixelStack.push({ x, y });

    while (!pixelStack.empty()) {
        auto [x, y] = pixelStack.top();
        pixelStack.pop();
        newColor = color();

        if (getPixel(x, y) == oldColor) {
            setPixel(x, y);

            // Push neighboring pixels to the stack
            if (x > 0 && getPixel(x - 1, y) == oldColor) {
                pixelStack.push({ x - 1, y });
            }
            if (x < m_dimension.width && getPixel(x + 1, y) == oldColor) {
                pixelStack.push({ x + 1, y });
            }
            if (y > 0 && getPixel(x, y - 1) == oldColor) {
                pixelStack.push({ x, y - 1 });
            }
            if (y < m_dimension.height && getPixel(x, y + 1) == oldColor) {
                pixelStack.push({ x, y + 1 });
            }
        }
    }
}

} // namespace sdlc