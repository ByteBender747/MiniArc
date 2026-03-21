#include "AudioDataBuffer.hpp"
#include "PathHelper.hpp"
#include "AppState.hpp"

#include <iostream>
#include <SDL3/SDL_audio.h>

namespace sdlc
{

AudioDataBufferPtr loadWave(sdlc::AppState* state, const std::filesystem::path &filePath)
{
    return std::make_unique<sdlc::AudioDataBuffer>(
        AudioDataBuffer(state->audio.audioSpec,
            resolveRelativeToExe(filePath).c_str()));
}

AudioDataBuffer::AudioDataBuffer(const AudioDataBuffer &other)
    : m_useSDL_free(false),
      m_size(other.m_size)
{
    m_data = new uint8_t[other.m_size];
    memcpy(m_data, other.m_data, m_size);
}

AudioDataBuffer::AudioDataBuffer(AudioDataBuffer &&other) noexcept
    : m_useSDL_free(other.m_useSDL_free),
      m_data(other.m_data),
      m_size(other.m_size)
{
    other.m_data = nullptr;
    other.m_size = 0;
}

AudioDataBuffer::AudioDataBuffer(uint32_t size)
    : m_useSDL_free(false), m_size(size)
{
    m_data = new uint8_t[size];
}

AudioDataBuffer::AudioDataBuffer(const SDL_AudioSpec& deviceSpec, const char *waveFileName)
    : m_useSDL_free(true)
{
    SDL_AudioSpec waveSpec;
    if (!SDL_LoadWAV(waveFileName, &waveSpec, &m_data, &m_size)) {
        std::cerr << "Error: Unable to load wav file: " << waveFileName << std::endl;
        m_data = nullptr;
        m_size = 0;
    } else {
        if (waveSpec.format != deviceSpec.format) {
            int destLen = 0;
            Uint8* destData = nullptr;
            if (!SDL_ConvertAudioSamples(&waveSpec, m_data,
                static_cast<int>(m_size), &deviceSpec, &destData, &destLen)) {
                std::cerr << "Error: Unable to convert audio samples into device format" << std::endl;
            } else {
                SDL_free(m_data);
                m_data = destData;
                m_size = destLen;
            }
        }
    }
}

sdlc::AudioDataBuffer::~AudioDataBuffer()
{
    if (m_data && m_size > 0) {
        if (m_useSDL_free) {
            SDL_free(m_data);
        } else {
            delete [] m_data;
        }
    }
}

}