#pragma once

#include <memory>
#include <filesystem>
#include <SDL3/SDL_audio.h>

namespace sdlc
{

class AudioDataBuffer
{
public:
    AudioDataBuffer() = default;
    AudioDataBuffer(uint32_t size);
    AudioDataBuffer(const SDL_AudioSpec &deviceSpec, const std::filesystem::path &waveFileName);
    AudioDataBuffer(const AudioDataBuffer &other);
    AudioDataBuffer(AudioDataBuffer &&other) noexcept;
    virtual ~AudioDataBuffer();
    bool loadWave(const SDL_AudioSpec& deviceSpec, const std::filesystem::path &filePath);

    [[nodiscard]] const void * data() const {
        return m_data;
    }
    [[nodiscard]] uint32_t size() const {
        return m_size;
    }

protected:
    void freeBuffer();

private:
    bool m_useSDL_free{false};
    uint8_t* m_data{nullptr};
    uint32_t m_size{0};
};

using AudioDataBufferPtr = std::unique_ptr<AudioDataBuffer>;

struct AppState;

AudioDataBufferPtr loadWaveRelative(AppState* state, const std::filesystem::path &filePath);
bool loadWaveRelative(AudioDataBuffer& buf, AppState *state, const std::filesystem::path &filePath);

}
