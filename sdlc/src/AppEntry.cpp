#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <string>

#include "AppState.hpp"
#include "Config.hpp"

#ifdef SCENE_HEADER
#include SCENE_HEADER
#endif

using namespace sdlc;

Scene::Scene(AppState *appState)
    : manager(appState), appState(appState)
{
}

static void EnumRenderDrivers()
{
    for (int i = 0; i < SDL_GetNumRenderDrivers(); ++i) {
        SDL_Log("RenderDriver(%d): %s", i, SDL_GetRenderDriver(i));
    }
}

SDL_AppResult SDL_AppInit(void** appState, int argc, char** argv)
{
    auto state = new AppState();
    *appState = state;
    SDL_InitFlags flags = SDL_INIT_VIDEO | SDL_INIT_EVENTS;

#ifdef AUDIO_FORMAT
    flags |= SDL_INIT_AUDIO;
#endif

    int result = SDL_InitSubSystem(flags);
    std::string subSystems;
    if (flags & SDL_INIT_VIDEO) subSystems += "Video ";
    if (flags & SDL_INIT_AUDIO) subSystems += "Audio ";
    if (flags & SDL_INIT_EVENTS) subSystems += "Events ";
    if(result < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDL_InitSubSystem() failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_Log("Initialized sub-systems: %s", subSystems.c_str());

    SDL_Log("Creating application window %dx%d with creation flags: 0x%x", WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FLAGS);
    state->window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FLAGS);
    if (!state->window) {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDL_CreateWindow() failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    EnumRenderDrivers();
    state->renderer = SDL_CreateRenderer(state->window, nullptr);
    if (!state->renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDL_CreateRenderer() failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_Log("Current render driver: %s", SDL_GetRendererName(state->renderer));

#ifdef TTF_INIT
    if (!TTF_Init()) {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "TTF_Init() failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
#endif

#ifdef RENDER_VSYNC
    SDL_SetRenderVSync(state->renderer, RENDER_VSYNC);
    SDL_Log("Enabled v-sync: %d", RENDER_VSYNC);
#endif
#if defined(RENDER_LOGICAL_WIDTH) && defined(RENDER_LOGICAL_HEIGHT)
    SDL_SetRenderLogicalPresentation(state->renderer, RENDER_LOGICAL_WIDTH, RENDER_LOGICAL_HEIGHT, SDL_LOGICAL_PRESENTATION_STRETCH);
    SDL_Log("Logical presentation size is %dx%d", RENDER_LOGICAL_WIDTH, RENDER_LOGICAL_HEIGHT);
#endif

#ifdef AUDIO_FORMAT
    state->audio.audioSpec.freq = AUDIO_FREQ;
    state->audio.audioSpec.format = AUDIO_FORMAT;
    state->audio.audioSpec.channels = AUDIO_CHANNELS;
    SDL_Log("Initializing audio device with frequency: %d, channels: %d, format: 0x%x", AUDIO_FREQ, AUDIO_CHANNELS, AUDIO_FORMAT);
    state->audio.device.id = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &state->audio.audioSpec);
    if (!state->audio.device.id) {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDL_OpenAudioDevice() failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    std::vector<SDL_AudioStream*> streamStack;
    for (int i = 0; i < AUDIO_NUM_STREAMS; ++i) {
        state->audio.stream[i].create(state->audio.audioSpec, state->audio.audioSpec);
        streamStack.emplace_back(state->audio.stream[i].stream());
    }
    if (!SDL_BindAudioStreams(state->audio.device.id, streamStack.data(), streamStack.size())) {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "SDL_BindAudioStreams() failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
#endif

#ifdef SCENE_CLASS
    state->scene = std::make_unique<SCENE_CLASS>(state);
#else
#error Default scene must be defined
#endif

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appState, SDL_Event* event)
{
    auto state = static_cast<AppState*>(appState);
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    state->scene->manager.handleEvent(*event);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appState)
{
    auto state = static_cast<AppState*>(appState);
    double now = SDL_GetPerformanceCounter() / static_cast<double>(SDL_GetPerformanceFrequency());
    if (state->timeStamp > 0) {
        state->deltaTime = static_cast<float>(now - state->timeStamp);
    }
    state->timeStamp = now;

    SDL_SetRenderDrawColor(
        state->renderer,
        state->clearColor.r, state->clearColor.g, state->clearColor.b, state->clearColor.a
    );
    SDL_RenderClear(state->renderer);

    state->scene->manager.addLayersDeferred();
    state->scene->manager.update(state->deltaTime);
    state->scene->manager.render(state->renderer);

    SDL_RenderPresent(state->renderer);

    ++state->iterations;
    return state->isRunning ? SDL_APP_CONTINUE : SDL_APP_SUCCESS;
}

void SDL_AppQuit(void* appState, SDL_AppResult result)
{
    auto state = static_cast<AppState*>(appState);
    state->scene.release();
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
    SDL_Log("Application exited with code: %d", result);
    SDL_Quit();
}