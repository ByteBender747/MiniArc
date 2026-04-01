#pragma once

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

#include <array>
#include <memory>
#include <string>
#include <cstdint>
#include <unordered_map>

#include "AppLayer.hpp"
#include "Keyboard.hpp"
#include "AudioDevice.hpp"
#include "AudioStream.hpp"

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
    Uint64 iterations{0};
    double timeStamp{0};
    float deltaTime{0};
    bool isRunning{true};
    char **argv{nullptr};
    int argc{0};
    SDL_Color clearColor{0, 0, 0, 255};
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