#pragma once

#include <SDL3/SDL_audio.h>

namespace sdlc
{

struct AudioDevice {
    SDL_AudioDeviceID id{0};
    void pause() const {
        SDL_PauseAudioDevice(id);
    }
    void resume() const {
        SDL_ResumeAudioDevice(id);
    }
};

}
