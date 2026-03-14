#pragma once

#include <SDL3/SDL_keyboard.h>

namespace sdl
{

static inline bool keyPressed(SDL_Scancode code)
{
    const bool* keys = SDL_GetKeyboardState(nullptr);
    if (keys && code < SDL_SCANCODE_COUNT) {
        return keys[code];
    } else {
        return false;
    }
}

} // namespace sdl