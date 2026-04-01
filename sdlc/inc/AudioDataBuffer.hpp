#pragma once

#include <cstdint>
#include <memory>
#include <filesystem>
#include <SDL3/SDL_audio.h>
#include <sys/types.h>

namespace sdlc
{

class AudioDataBuffer
{
public:
    AudioDataBuffer() = default;
    AudioDataBuffer(uint32_t size, uint8_t channels, uint32_t sampleRate, SDL_AudioFormat format);
    AudioDataBuffer(const SDL_AudioSpec &deviceSpec, const std::filesystem::path &waveFileName);
    AudioDataBuffer(const AudioDataBuffer &other);
    AudioDataBuffer(AudioDataBuffer &&other) noexcept;
    virtual ~AudioDataBuffer();
    float getPlayTime() const;
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
    SDL_AudioSpec m_waveSpec{};
    bool m_useSDL_free{false};
    uint8_t* m_data{nullptr};
    uint32_t m_size{0};
};

using AudioDataBufferPtr = std::unique_ptr<AudioDataBuffer>;

struct AppState;

AudioDataBufferPtr LoadWaveRelative(AppState* state, const std::filesystem::path &filePath);
bool LoadWaveRelative(AudioDataBuffer& buf, AppState *state, const std::filesystem::path &filePath);

} // namespace sdlc
