#pragma once

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

#include <array>
#include <memory>
#include <string>
#include <cstdint>
#include <unordered_map>

#include "Config.hpp"
#include "AppLayer.hpp"
#include "Keyboard.hpp"
#include "AudioDevice.hpp"
#include "AudioStream.hpp"
#include "StopWatch.hpp"

namespace sdlc
{

union Property
{
    uint8_t data[8] {0, 0, 0, 0, 0, 0, 0, 0};
    void* pointer;
    bool boolean;
    int64_t integer;
    float floatValue;
    double doubleValue;
};

struct Scene
{
    Scene(AppState *appState);
    AppState *appState;
    LayerManager manager;
    std::unordered_map<std::string, sdlc::Property> properties;
};

struct AppState
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    int currentFPS;
    int frameCounter{0};
    int maxFPS{0};
    StopWatch timeStamp;
    StopWatch frameTimer;
    float deltaTime{0};
    float timeScale{1};
    bool showFPS{false};
    bool isRunning{true};
    char **argv{nullptr};
    int argc{0};
    SDL_Color clearColor{0, 0, 0, 255};
    AppConfig config;
    std::unique_ptr<Scene> scene;
    std::unordered_map<std::string, sdlc::Property> properties;
    struct {
        KeyMap keys;
    } input;
#ifdef AUDIO_FORMAT
    struct {
        AudioDevice device;
        SDL_AudioSpec audioSpec{};
        std::array<AudioStream, AUDIO_NUM_STREAMS> stream;
    } audio;
#endif
    void exchangeScene(Scene *scenePtr);
};

} // namespace sdlc