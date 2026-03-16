#pragma once

#include <string>
#include <unordered_map>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_scancode.h>

namespace sdlc
{

static inline bool getKeyState(SDL_Scancode code)
{
    const bool* keys = SDL_GetKeyboardState(nullptr);
    if (keys && code < SDL_SCANCODE_COUNT) {
        return keys[code];
    } else {
        return false;
    }
}

class KeyMap
{
public:
    void mapKey(const std::string& keyName, SDL_Scancode code);
    bool down(const std::string& keyName);
    bool up(const std::string& keyName);
    bool risingEdge(const std::string& keyName);
    bool fallingEdge(const std::string& keyName);
    void clearStates();
public:
    struct KeyState {
        SDL_Scancode scanCode{SDL_SCANCODE_UNKNOWN};
        bool previousState{false};
    };
private:
    std::unordered_map<std::string, KeyState> m_keyMap;
};

} // namespace sdlc