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

AudioStream::AudioStream(const SDL_AudioSpec &inputFmt, const SDL_AudioSpec &outputFmt)
    : m_inputFmt(inputFmt), m_outputFmt(outputFmt)
{
    m_stream = SDL_CreateAudioStream(&m_inputFmt, &m_outputFmt);
    if (!m_stream) {
        std::cerr << "Error: AudioStream::AudioStream(): could not create audio stream" << std::endl;
    }
}

AudioStream::~AudioStream()
{
    if (m_stream) {
        SDL_DestroyAudioStream(m_stream);
    }
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
