#pragma once

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

#include <map>
#include <string>
#include <cstdint>
#include <functional>
#include <unordered_map>

namespace sdlc
{

union Property {
    uint8_t data[8] {0, 0, 0, 0, 0, 0, 0, 0};
    void* pointer;
    int64_t integer;
    float floatValue;
    double doubleValue;
};

struct AppState {
    using EventHandler = std::function<void(AppState* state, SDL_Event* event)>;
    using IterateHandler = std::function<void(AppState* state)>;
    SDL_Window* window;
    SDL_Renderer* renderer;
    Uint64 iterations{0};
    double timeStamp{0};
    double deltaTime{0};
    bool isRunning{true};
    SDL_Color clearColor{0, 0, 0, 255};
    std::map<int, EventHandler> eventHandler;
    std::map<int, IterateHandler> iterateHandler;
    std::unordered_map<std::string, sdlc::Property> properties;
};

} // namespace sdlc