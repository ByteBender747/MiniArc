#include "Timer.hpp"

#include <cassert>
#include <utility>
#include <SDL3/SDL.h>

namespace sdlc
{

static Uint32 TimerCallbackProxy(void *userdata, SDL_TimerID timerID, Uint32 interval)
{
    auto timer = static_cast<Timer*>(userdata);
    assert(timer);
    if (timer->timerId() == timerID) {
        timer->setInterval(interval);
        return timer->elapsed();
    }
    return interval;
}

Timer::Timer(Uint32 interval, Callback callback)
    : m_interval(interval)
    , m_callback(std::move(callback))
{
    m_timerID = SDL_AddTimer(interval, TimerCallbackProxy, this);
    if (!m_timerID) {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDL_AddTimer() failed: %s", SDL_GetError());
    } else {
        SDL_Log("Attached timer with ID: %d, interval: %d", m_timerID, interval);
    }
}

Timer::Timer(const Timer &other)
    : Timer(other.interval(), other.m_callback)
{
}

Timer::Timer(Timer &&other) noexcept
    : m_interval(other.m_interval)
    , m_timerID(other.m_timerID)
{
    m_callback = std::move(other.m_callback);
    other.m_timerID = 0;
}

Timer::~Timer()
{
    if (m_timerID) {
        SDL_RemoveTimer(m_timerID);
        SDL_Log("Removed timer with ID: %d", m_timerID);
    }
}

Uint32 Timer::elapsed()
{
    if (m_callback) {
        m_interval = m_callback();
    }
    return m_interval;
}

} // namespace sdlc