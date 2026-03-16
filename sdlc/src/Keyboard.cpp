#include "Keyboard.hpp"

namespace sdlc
{

void KeyMap::mapKey(const std::string& keyName, SDL_Scancode code)
{
    m_keyMap[keyName] = KeyState{code, false};
}

bool KeyMap::down(const std::string& keyName)
{
    auto it = m_keyMap.find(keyName);
    if (it == m_keyMap.end()) {
        return false;
    }
    return getKeyState(it->second.scanCode);
}

bool KeyMap::up(const std::string& keyName)
{
    return !down(keyName);
}

void KeyMap::clearStates()
{
    for (auto key : m_keyMap) {
        key.second.previousState = false;
    }
}

bool KeyMap::risingEdge(const std::string& keyName)
{
    auto it = m_keyMap.find(keyName);
    if (it == m_keyMap.end()) {
        return false;
    }
    
    bool currentState = getKeyState(it->second.scanCode);
    bool wasRisingEdge = !it->second.previousState && currentState;
    it->second.previousState = currentState;
    
    return wasRisingEdge;
}

bool KeyMap::fallingEdge(const std::string& keyName)
{
    auto it = m_keyMap.find(keyName);
    if (it == m_keyMap.end()) {
        return false;
    }
    
    bool currentState = getKeyState(it->second.scanCode);
    bool wasFallingEdge = it->second.previousState && !currentState;
    it->second.previousState = currentState;
    
    return wasFallingEdge;
}

} // namespace sdlc