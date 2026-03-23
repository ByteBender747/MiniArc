#include "AudioStream.hpp"

#include <iostream>
#include <ostream>

namespace sdlc
{

static void streamCallback(void *userdata, const void *buf, int buflen)
{
    auto *stream = static_cast<AudioStream *>(userdata);
    stream->audioStreamFinished(buf, buflen);
}

AudioStream::AudioStream(const AudioStream &other)
    : m_gain(other.m_gain)
    , m_hasData(other.m_hasData)
    , m_callback(other.m_callback)
    , m_inputFmt(other.m_inputFmt)
    , m_outputFmt(other.m_outputFmt)
{
    m_stream = SDL_CreateAudioStream(&m_inputFmt, &m_outputFmt);
    if (!m_stream) {
        std::cerr << "Error: AudioStream::AudioStream(): could not create audio stream" << std::endl;
    }
}

AudioStream::AudioStream(AudioStream &&other) noexcept
    : m_gain(other.m_gain)
    , m_hasData(other.m_hasData)
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
        std::cerr << "Error: AudioStream::AudioStream(): could not create audio stream" << std::endl;
        std::cerr << "SDL_GetError: " << SDL_GetError() << std::endl;
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

void AudioStream::put(const void *data, int len)
{
    if (m_stream) {
        if (data && len > 0) {
            SDL_PutAudioStreamData(m_stream, data, len);
            m_hasData = true;
        }
    }
}

void AudioStream::clear()
{
    if (m_stream) {
        SDL_ClearAudioStream(m_stream);
        m_hasData = false;
    }
}

void AudioStream::emplace(const void *data, int len)
{
    if (m_stream) {
        if (data && len > 0) {
            SDL_PutAudioStreamDataNoCopy(m_stream, data, len, streamCallback, this);
            m_hasData = true;
        }
    }
}

void AudioStream::audioStreamFinished(const void *buf, int len)
{
    if (m_callback) {
        m_callback(*this, buf, len);
    }
    m_hasData = false;
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
