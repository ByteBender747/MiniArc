#include "Utility.hpp"
#include "AppState.hpp"
#include "AudioDataBuffer.hpp"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_audio.h>

namespace sdlc
{

AudioDataBufferPtr LoadWaveRelative(AppState *state, const std::filesystem::path &filePath)
{
    return std::make_unique<sdlc::AudioDataBuffer>(
        AudioDataBuffer(state->audio.audioSpec, ResolveRelativeToExe(filePath)));
}

bool LoadWaveRelative(AudioDataBuffer& buf, AppState *state, const std::filesystem::path &filePath)
{
    return buf.loadWave(state->audio.audioSpec, ResolveRelativeToExe(filePath));
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

AudioDataBuffer::AudioDataBuffer(uint32_t size, u_int8_t channels, uint32_t sampleRate, SDL_AudioFormat format)
    : m_size(size)
{
    m_data = new uint8_t[size];
    m_waveSpec.channels = channels;
    m_waveSpec.freq = sampleRate;
    m_waveSpec.format = format;
}

float AudioDataBuffer::getPlayTime() const
{
    int sampleSize;
    switch (m_waveSpec.format) {
        case SDL_AUDIO_U8:
        case SDL_AUDIO_S8:
            sampleSize = 1;
            break;
        case SDL_AUDIO_S16LE:
        case SDL_AUDIO_S16BE:
            sampleSize = 2;
            break;
        case SDL_AUDIO_S32LE:
        case SDL_AUDIO_S32BE:
        case SDL_AUDIO_F32LE:
        case SDL_AUDIO_F32BE:
            sampleSize = 4;
            break;
        default:
            sampleSize = 0;
    }
    if (m_data && m_size && sampleSize && m_waveSpec.channels && m_waveSpec.freq) {
        return static_cast<float>(m_size) / m_waveSpec.freq / m_waveSpec.channels / sampleSize;
    }
    return 0;
}

bool AudioDataBuffer::loadWave(const SDL_AudioSpec &deviceSpec, const std::filesystem::path &filePath)
{
    bool result = true;
    freeBuffer(); // Delete old data first if there are any
    SDL_Log("Loading WAV file: %s", filePath.c_str());
    if (!SDL_LoadWAV(filePath.c_str(), &m_waveSpec, &m_data, &m_size)) {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Unable to load WAV file: %s", filePath.c_str());
        m_data = nullptr;
        m_size = 0;
        result = false;
    } else {
        if (m_waveSpec.format != deviceSpec.format) {
            int destLen = 0;
            Uint8 *destData = nullptr;
            if (!SDL_ConvertAudioSamples(&m_waveSpec, m_data,
                                         static_cast<int>(m_size), &deviceSpec, &destData, &destLen)) {
                SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Unable to convert audio samples into device specific format");
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

} // namespace sdlc
