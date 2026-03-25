#include "AudioDataBuffer.hpp"
#include "Utility.hpp"
#include "AppState.hpp"

#include <iostream>
#include <SDL3/SDL_audio.h>

namespace sdlc
{

AudioDataBufferPtr loadWaveRelative(AppState *state, const std::filesystem::path &filePath)
{
    return std::make_unique<sdlc::AudioDataBuffer>(
        AudioDataBuffer(state->audio.audioSpec, resolveRelativeToExe(filePath)));
}

bool loadWaveRelative(AudioDataBuffer& buf, AppState *state, const std::filesystem::path &filePath)
{
    return buf.loadWave(state->audio.audioSpec, resolveRelativeToExe(filePath));
}

AudioDataBuffer::AudioDataBuffer(const AudioDataBuffer &other)
    : m_size(other.m_size)
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
    : m_size(size)
{
    m_data = new uint8_t[size];
}

bool AudioDataBuffer::loadWave(const SDL_AudioSpec &deviceSpec, const std::filesystem::path &filePath)
{
    bool result = true;
    SDL_AudioSpec waveSpec;
    freeBuffer(); // Delete old data first if there are any
    if (!SDL_LoadWAV(filePath.c_str(), &waveSpec, &m_data, &m_size)) {
        std::cerr << "Error: Unable to load wav file: " << filePath.c_str() << std::endl;
        m_data = nullptr;
        m_size = 0;
        result = false;
    } else {
        if (waveSpec.format != deviceSpec.format) {
            int destLen = 0;
            Uint8 *destData = nullptr;
            if (!SDL_ConvertAudioSamples(&waveSpec, m_data,
                                         static_cast<int>(m_size), &deviceSpec, &destData, &destLen)) {
                std::cerr << "Error: Unable to convert audio samples into device format" << std::endl;
                result = false;
            } else {
                SDL_free(m_data);
                m_data = destData;
                m_size = destLen;
            }
        }
    }
    return result;
}

AudioDataBuffer::AudioDataBuffer(const SDL_AudioSpec &deviceSpec, const std::filesystem::path &waveFileName)
    : m_useSDL_free(true)
{
    loadWave(deviceSpec, waveFileName);
}

void AudioDataBuffer::freeBuffer()
{
    if (m_data && m_size > 0) {
        if (m_useSDL_free) {
            SDL_free(m_data);
        } else {
            delete [] m_data;
        }
        m_data = nullptr;
        m_size = 0;
        m_useSDL_free = false;
    }
}

AudioDataBuffer::~AudioDataBuffer()
{
    freeBuffer();
}

}
