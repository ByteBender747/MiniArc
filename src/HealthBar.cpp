#include <SDL3/SDL_render.h>

#include "Rect.hpp"
#include "HealthBar.hpp"

HealthBar::HealthBar(sdlc::AppState* appState, const sdlc::Rect<float>& rect)
    : m_appState(appState), m_rect(rect)
    , m_barColor(255, 0, 0, 255), m_frameColor(255, 255, 255, 255)
{
    m_width = m_rect.w;
}

void HealthBar::render()
{
    SDL_SetRenderDrawColor(m_appState->renderer, m_barColor.r, m_barColor.g, m_barColor.b, m_barColor.a);
    m_rect.w = m_width * m_percentage;
    SDL_RenderFillRect(m_appState->renderer, &m_rect);
    m_rect.w = m_width;
    SDL_SetRenderDrawColor(m_appState->renderer, m_frameColor.r, m_frameColor.g, m_frameColor.b, m_frameColor.a);
    SDL_RenderRect(m_appState->renderer, &m_rect);
}

void HealthBar::setValue(int value)
{
    if (value > 0) {
        m_percentage = static_cast<float>(value) / m_range;
    } else {
        m_percentage = 0;
    }
}

int HealthBar::getValue()
{
    return m_range * m_percentage;
}