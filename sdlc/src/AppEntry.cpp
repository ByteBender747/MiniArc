#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "AppState.hpp"
#include "Config.hpp"

using namespace sdlc;

void APP_INIT(AppState* state, int argc, char** argv);
void APP_EXIT(AppState* state);

#include <iostream>

SDL_AppResult SDL_AppInit(void** appState, int argc, char** argv)
{
    auto state = new AppState();
    *appState = state;
    SDL_InitFlags flags = SDL_INIT_VIDEO | SDL_INIT_EVENTS;

#ifdef AUDIO_FORMAT
    flags |= SDL_INIT_AUDIO;
#endif

    int result = SDL_InitSubSystem(flags);
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

#ifdef AUDIO_FORMAT
    state->audio.audioSpec.freq = AUDIO_FREQ;
    state->audio.audioSpec.format = AUDIO_FORMAT;
    state->audio.audioSpec.channels = AUDIO_CHANNELS;
    state->audio.device.id = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &state->audio.audioSpec);
    if (!state->audio.device.id) {
        std::cerr << "SDL_OpenAudioDevice() failed: " << SDL_GetError() << std::endl;
        return SDL_APP_FAILURE;
    }
    std::vector<SDL_AudioStream*> streamStack;
    for (int i = 0; i < AUDIO_NUM_STREAMS; ++i) {
        state->audio.stream[i] = std::make_unique<AudioStream>(state->audio.audioSpec, state->audio.audioSpec);
        streamStack.emplace_back(state->audio.stream[i]->stream());
    }
    SDL_BindAudioStreams(state->audio.device.id, streamStack.data(), streamStack.size());
#endif

    APP_INIT(state, argc, argv); // Entry point defined via cmake

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appState, SDL_Event* event)
{
    auto state = static_cast<AppState*>(appState);
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    for (auto& handler : state->eventHandler) {
        handler.second(state, event);
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appState)
{
    auto state = static_cast<AppState*>(appState);
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

    for (auto& handler : state->iterateHandler) {
        handler.second(state);
    }

    SDL_RenderPresent(state->renderer);

    ++state->iterations;
    return state->isRunning ? SDL_APP_CONTINUE : SDL_APP_SUCCESS;
}

void SDL_AppQuit(void* appState, SDL_AppResult result)
{
    auto state = static_cast<AppState*>(appState);
    APP_EXIT(state);
    if (state->renderer) {
        SDL_DestroyRenderer(state->renderer);
    }
    if (state->window) {
        SDL_DestroyWindow(state->window);
    }
#ifdef AUDIO_FORMAT
    SDL_CloseAudioDevice(state->audio.device.id);
#endif
    delete state;
    SDL_Quit();
}