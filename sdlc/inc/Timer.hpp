#pragma once

#include <functional>
#include <SDL3/SDL_timer.h>

namespace sdlc {

class Timer
{
public:
    using Callback = std::function<Uint32()>;
    Timer(Uint32 interval, Callback callback = nullptr);
    Timer(const Timer &other);
    Timer(Timer &&other) noexcept;
    virtual ~Timer();
    virtual Uint32 elapsed();
    [[nodiscard]] Callback callback() const
    {
        return m_callback;
    }
    void setCallback(const Callback &callback)
    {
        m_callback = callback;
    }
    [[nodiscard]] SDL_TimerID timerId() const
    {
        return m_timerID;
    }
    [[nodiscard]] Uint32 interval() const
    {
        return m_interval;
    }
    void setInterval(Uint32 interval)
    {
        m_interval = interval;
    }
    [[nodiscard]] bool isRunning() const
    {
        return m_timerID != 0;
    }

private:
    Uint32 m_interval;
    SDL_TimerID m_timerID;
    Callback m_callback{nullptr};
};

} // sdlc