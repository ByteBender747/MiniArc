#pragma once

#include <functional>
#include <SDL3/SDL_audio.h>

#include "AudioDataBuffer.hpp"

namespace sdlc
{

class AudioStream
{
public:
    using AudioStreamFinishedCallback = std::function<void(AudioStream&, const void *, int len)>;

public:
    AudioStream() = default;
    AudioStream(const AudioStream &other);
    AudioStream(AudioStream &&other) noexcept;
    AudioStream(const SDL_AudioSpec& inputFmt, const SDL_AudioSpec& outputFmt);
    virtual ~AudioStream();
    virtual void audioStreamFinished(const void* buf, int len);
    void create(const SDL_AudioSpec& inputFmt, const SDL_AudioSpec& outputFmt);
    void put(const void* data, int len, bool clear = false);
    int get(void* buf, int len) const;
    void emplace(const void* data, int len, bool clear = false);
    void setGain(float gain);
    void clear();
    void put(const AudioDataBuffer& buffer, bool clear = false) {
        put(buffer.data(), buffer.size(), clear);
    }
    void emplace(const AudioDataBuffer& buffer, bool clear = false) {
        emplace(buffer.data(), buffer.size(), clear);
    }
    [[nodiscard]] int bytesQueued() const;
    [[nodiscard]] float gain() const {
        return m_gain;
    }
    [[nodiscard]] SDL_AudioStream* stream() const {
        return m_stream;
    }
    [[nodiscard]] const SDL_AudioSpec& inputFormat() const {
        return m_inputFmt;
    }
    [[nodiscard]] const SDL_AudioSpec& outputFormat() const {
        return m_outputFmt;
    }
private:
    float m_gain{1};
    AudioStreamFinishedCallback m_callback;
    SDL_AudioSpec m_inputFmt{}, m_outputFmt{};
    SDL_AudioStream* m_stream{nullptr};
};

}
