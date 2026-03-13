#pragma once

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

#include <map>
#include <functional>
#include <string>

#ifndef WINDOW_WIDTH
#define WINDOW_WIDTH 640
#endif

#ifndef WINDOW_HEIGHT
#define WINDOW_HEIGHT 480
#endif

#ifndef WINDOW_FLAGS
#define WINDOW_FLAGS 0
#endif

#ifndef WINDOW_TITLE
#define WINDOW_TITLE "SDL3 Application"
#endif

#ifndef APP_INIT
#define APP_INIT
#warning Application entry point not defined
#endif

#ifndef APP_EXIT
#define APP_EXIT
#warning Application exit point not defined
#endif

struct AppState {
    using EventHandler = std::function<void(AppState* state, SDL_Event* event)>;
    using IterateHandler = std::function<void(AppState* state)>;
    SDL_Window* window;
    SDL_Renderer* renderer;
    Uint64 iterations{0};
    double timeStamp{0};
    double deltaTime{0};
    bool isRunning{true};
    void* userdata{nullptr};
    SDL_Color clearColor{0, 0, 0, 255};
    std::map<int, EventHandler> eventHandler;
    std::map<int, IterateHandler> iterateHandler;
};