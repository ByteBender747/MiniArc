#pragma once

#include <cstdint>

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_surface.h>

#include "Rect.hpp"
#include "Dimension.hpp"
#include "ColorConverter.hpp"

namespace sdlc
{

class Painter
{
public:
    void line(int x0, int y0, int x1, int y1);
    void circle(int x0, int y0, int radius);
    void ellipse(int xm, int ym, int a, int b);
    void floodFill(int x, int y);
    void moveTo(int x, int y);
    void lineTo(int x, int y);
    const Dimension<uint32_t>& dimension() const {
        return m_dimension;
    }
    void setColor(const RGBA& value) {
        m_color = value;
    }
    const RGBA& color() const {
        return m_color;
    }
public:
    virtual void setPixel(int x, int y) = 0;
    virtual RGBA getPixel(int x, int y) = 0;
protected:
    void setDimension(uint32_t width, uint32_t height);
private:
    RGBA m_color{0, 0, 0, 255};
    Dimension<uint32_t> m_dimension;
    Point2D<int> m_cursor;
};

class SurfacePainter : public Painter
{
public:
    SurfacePainter(SDL_Surface* surface);
    void setPixel(int x, int y) override;
    RGBA getPixel(int x, int y) override;
private:
    SDL_Surface* m_surface;
    SDL_PixelFormatDetails m_pixelFormat;
};

} // namespace sdlc