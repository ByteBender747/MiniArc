#include "AudioStream.hpp"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_error.h>

namespace sdlc
{

static void streamCallback(void *userdata, const void *buf, int buflen)
{
    auto *stream = static_cast<AudioStream *>(userdata);
    stream->audioStreamFinished(buf, buflen);
}

AudioStream::AudioStream(const AudioStream &other)
    : m_gain(other.m_gain)
    , m_callback(other.m_callback)
    , m_inputFmt(other.m_inputFmt)
    , m_outputFmt(other.m_outputFmt)
{
    m_stream = SDL_CreateAudioStream(&m_inputFmt, &m_outputFmt);
    if (!m_stream) {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "SDL_CreateAudioStream() failed: %s", SDL_GetError());
    }
}

AudioStream::AudioStream(AudioStream &&other) noexcept
    : m_gain(other.m_gain)
    , m_callback(std::move(other.m_callback))
    , m_inputFmt(other.m_inputFmt)
    , m_outputFmt(other.m_outputFmt)
{
    m_stream = other.m_stream;
    other.m_stream = nullptr;
}

AudioStream::AudioStream(const SDL_AudioSpec &inputFmt, const SDL_AudioSpec &outputFmt)
{
    create(inputFmt, outputFmt);
}

void AudioStream::create(const SDL_AudioSpec &inputFmt, const SDL_AudioSpec &outputFmt)
{
    if (m_stream) {
        SDL_DestroyAudioStream(m_stream);
        m_stream = nullptr;
    }
    m_inputFmt = inputFmt;
    m_outputFmt = outputFmt;
    m_stream = SDL_CreateAudioStream(&m_inputFmt, &m_outputFmt);
    if (!m_stream) {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "SDL_CreateAudioStream() failed: %s", SDL_GetError());
    }
}

AudioStream::~AudioStream()
{
    if (m_stream) {
        SDL_DestroyAudioStream(m_stream);
    }
}

int AudioStream::bytesQueued() const
{
    if (m_stream) {
        return SDL_GetAudioStreamQueued(m_stream);
    }
    return 0;
}

void AudioStream::put(const void *data, int len, bool clear)
{
    if (m_stream) {
        if (clear) {
            SDL_ClearAudioStream(m_stream);
        }
        if (data && len > 0) {
            SDL_PutAudioStreamData(m_stream, data, len);
        }
    }
}

void AudioStream::clear()
{
    if (m_stream) {
        SDL_ClearAudioStream(m_stream);
    }
}

void AudioStream::emplace(const void *data, int len, bool clear)
{
    if (m_stream) {
        if (clear) {
            SDL_ClearAudioStream(m_stream);
        }
        if (data && len > 0) {
            SDL_PutAudioStreamDataNoCopy(m_stream, data, len, streamCallback, this);
        }
    }
}

void AudioStream::audioStreamFinished(const void *buf, int len)
{
    if (m_callback) {
        m_callback(*this, buf, len);
    }
}

int AudioStream::get(void *buf, int len) const
{
    if (m_stream) {
        return SDL_GetAudioStreamData(m_stream, buf, len);
    } else {
        return 0;
    }
}

void AudioStream::setGain(float gain)
{
    m_gain = gain;
    if (m_stream) {
        SDL_SetAudioStreamGain(m_stream, m_gain);
    }
}

}
