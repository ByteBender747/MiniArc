#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_timer.h>

#include "TextInput.hpp"

namespace sdlc
{

TextInput::TextInput(SDL_Renderer *renderer, FontRenderer *font)
    : m_renderer(renderer)
      , m_font(font)
      , m_lastBlinkTime(SDL_GetTicks())
{
}

void TextInput::handleEvent(const SDL_Event &event)
{
    if (event.type == SDL_EVENT_TEXT_INPUT) {
        for (const char *p = event.text.text; *p; ++p) {
            if (*p >= 32 && *p <= 126) {
                m_text.insert(m_cursorPos, 1, *p);
                ++m_cursorPos;
            }
        }
        m_caretVisible = true;
        m_lastBlinkTime = SDL_GetTicks();
        return;
    }

    if (event.type != SDL_EVENT_KEY_DOWN)
        return;

    switch (event.key.scancode) {
        case SDL_SCANCODE_LEFT:
            if (m_cursorPos > 0) --m_cursorPos;
            break;
        case SDL_SCANCODE_RIGHT:
            if (m_cursorPos < m_text.size()) ++m_cursorPos;
            break;
        case SDL_SCANCODE_HOME:
            m_cursorPos = 0;
            break;
        case SDL_SCANCODE_END:
            m_cursorPos = m_text.size();
            break;
        case SDL_SCANCODE_BACKSPACE:
            if (m_cursorPos > 0) {
                m_text.erase(m_cursorPos - 1, 1);
                --m_cursorPos;
            }
            break;
        case SDL_SCANCODE_DELETE:
            if (m_cursorPos < m_text.size()) {
                m_text.erase(m_cursorPos, 1);
            }
            break;
        case SDL_SCANCODE_RETURN:
            if (m_callback) m_callback(m_text, true);
            break;
        case SDL_SCANCODE_ESCAPE:
            if (m_callback) m_callback(m_text, false);
            break;
        default:
            return;
    }

    m_caretVisible = true;
    m_lastBlinkTime = SDL_GetTicks();
}

void TextInput::render(float x, float y)
{
    m_font->renderText(x, y, m_text);

    Uint64 now = SDL_GetTicks();
    if (now - m_lastBlinkTime >= m_blinkInterval) {
        m_caretVisible = !m_caretVisible;
        m_lastBlinkTime = now;
    }

    if (!m_caretVisible)
        return;

    float caretX = x + m_font->measureText(std::string_view(m_text.data(), m_cursorPos)).width;
    float caretHeight = m_font->size() * m_font->scale();

    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(m_renderer, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(m_renderer, m_caretColor.r, m_caretColor.g, m_caretColor.b, m_caretColor.a);
    SDL_RenderLine(m_renderer, caretX, y, caretX, y + caretHeight);
    SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
}

void TextInput::setText(std::string_view text)
{
    m_text = text;
    m_cursorPos = m_text.size();
}

void TextInput::setCursorPos(size_t pos)
{
    m_cursorPos = pos <= m_text.size() ? pos : m_text.size();
}

void TextInput::startTextInput(SDL_Window *window)
{
    SDL_StartTextInput(window);
}

void TextInput::stopTextInput(SDL_Window *window)
{
    SDL_StopTextInput(window);
}

} // namespace sdlc
