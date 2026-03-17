#pragma once

#include <SDL3/SDL_rect.h>

#include "Rect.hpp"
#include "AppState.hpp"
#include "ColorConverter.hpp"

class HealthBar
{
public:
    HealthBar(sdlc::AppState* appState, const sdlc::Rect<float>& rect);
    void setRange(int range) { m_range = range; }
    int getRange() const { return m_range; }
    void setValue(int value);
    int getValue();
    void render();
    void setBarColor(const sdlc::RGBA& color) {
        m_barColor = color;
    }
    const sdlc::RGBA getBarColor() const {
        return m_barColor;
    }
    void setFrameColor(const sdlc::RGBA& color) {
        m_frameColor = color;
    }
    const sdlc::RGBA getFrameColor() const {
        return m_frameColor;
    }

private:
    int m_range{100};
    float m_width;
    float m_percentage{1};
    sdlc::RGBA m_barColor;
    sdlc::RGBA m_frameColor;
    SDL_FRect m_rect;
    sdlc::AppState* m_appState;
};