#pragma once

#include <string>
#include <string_view>
#include <functional>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>

#include "ColorConverter.hpp"
#include "FontRenderer.hpp"

namespace sdlc
{

class TextInput
{
public:
    using Callback = std::function<void(std::string_view, bool)>;

    TextInput(SDL_Renderer* renderer, FontRenderer* font);

    void handleEvent(const SDL_Event& event);
    void render(float x, float y);

    [[nodiscard]] const std::string& text() const { return m_text; }
    void setText(std::string_view text);

    [[nodiscard]] size_t cursorPos() const { return m_cursorPos; }
    void setCursorPos(size_t pos);

    void setCaretColor(const RGBA& color) { m_caretColor = color; }
    void setBlinkInterval(Uint32 ms) { m_blinkInterval = ms; }

    static void startTextInput(SDL_Window* window);
    static void stopTextInput(SDL_Window* window);

    [[nodiscard]] Callback callback() const { return m_callback; }
    void setCallback(const Callback &callback) { m_callback = callback; }

private:
    Callback m_callback{nullptr};
    SDL_Renderer* m_renderer;
    FontRenderer* m_font;
    std::string m_text;
    size_t m_cursorPos{0};
    RGBA m_caretColor{255, 255, 255, 255};
    Uint32 m_blinkInterval{500};
    Uint64 m_lastBlinkTime{0};
    bool m_caretVisible{true};
};

} // namespace sdlc
