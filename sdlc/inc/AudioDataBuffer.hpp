#pragma once

#include <memory>
#include <filesystem>
#include <SDL3/SDL_audio.h>

namespace sdlc
{

class AudioDataBuffer
{
public:
    AudioDataBuffer(uint32_t size);
    AudioDataBuffer(const SDL_AudioSpec& deviceSpec, const char* waveFileName);
    AudioDataBuffer(const AudioDataBuffer &other);
    AudioDataBuffer(AudioDataBuffer &&other) noexcept;
    virtual ~AudioDataBuffer();

public:
    [[nodiscard]] const void * data() const {
        return m_data;
    }
    [[nodiscard]] int size() const {
        return m_size;
    }

private:
    bool m_useSDL_free;
    uint8_t* m_data;
    uint32_t m_size;
};

using AudioDataBufferPtr = std::unique_ptr<AudioDataBuffer>;

struct AppState;

AudioDataBufferPtr loadWave(AppState* state, const std::filesystem::path &filePath);

}
