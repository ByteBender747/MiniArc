#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "AppState.hpp"

using namespace sdl;

#ifdef TTF_INIT
#include <SDL3_ttf/SDL_ttf.h>
#endif

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

void APP_INIT(AppState* state, int argc, char** argv);
void APP_EXIT(AppState* state);

#include <iostream>

SDL_AppResult SDL_AppInit(void** appState, int argc, char** argv)
{
    AppState* state = new AppState();
    *appState = state;

    int result = SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    if(result < 0) {
        std::cerr << "SDL_InitSubSystem() failed: " << SDL_GetError() << std::endl;
        return SDL_APP_FAILURE;
    }

    state->window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FLAGS);
    if (!state->window) {
        std::cerr << "SDL_CreateWindow() failed: " << SDL_GetError() << std::endl;
        return SDL_APP_FAILURE;
    }

    state->renderer = SDL_CreateRenderer(state->window, nullptr);
    if (!state->renderer) {
        std::cerr << "SDL_CreateRenderer() failed: " << SDL_GetError() << std::endl;
        return SDL_APP_FAILURE;
    }

#ifdef TTF_INIT
    if (!TTF_Init()) {
        std::cerr << "TTF_Init() failed: " << SDL_GetError() << std::endl;
        return SDL_APP_FAILURE;
    }
#endif

#ifdef RENDER_VSYNC
    SDL_SetRenderVSync(state->renderer, RENDER_VSYNC);
#endif
#if defined(RENDER_LOGICAL_WIDTH) && defined(RENDER_LOGICAL_HEIGHT)
    SDL_SetRenderLogicalPresentation(state->renderer, RENDER_LOGICAL_WIDTH, RENDER_LOGICAL_HEIGHT, SDL_LOGICAL_PRESENTATION_STRETCH);
#endif

    APP_INIT(state, argc, argv); // Entry point defined via cmake

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appState, SDL_Event* event)
{
    AppState* state = static_cast<AppState*>(appState);
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    for (auto handler : state->eventHandler) {
        handler.second(state, event);
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appState)
{
    AppState* state = static_cast<AppState*>(appState);
    double now = SDL_GetPerformanceCounter() / static_cast<double>(SDL_GetPerformanceFrequency());
    if (state->timeStamp > 0) {
        state->deltaTime = now - state->timeStamp;
    }
    state->timeStamp = now;

    SDL_SetRenderDrawColor(
        state->renderer,
        state->clearColor.r, state->clearColor.g, state->clearColor.b, state->clearColor.a
    );
    SDL_RenderClear(state->renderer);

    for (auto handler : state->iterateHandler) {
        handler.second(state);
    }

    SDL_RenderPresent(state->renderer);

    ++state->iterations;
    return state->isRunning ? SDL_APP_CONTINUE : SDL_APP_SUCCESS;
}

void SDL_AppQuit(void* appState, SDL_AppResult result)
{
    AppState* state = static_cast<AppState*>(appState);
    APP_EXIT(state);
    if (state->renderer) {
        SDL_DestroyRenderer(state->renderer);
    }
    if (state->window) {
        SDL_DestroyWindow(state->window);
    }
    delete state;
}