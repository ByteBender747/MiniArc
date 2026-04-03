#pragma once

#include <SDL3/SDL_timer.h>

namespace sdlc
{

class StopWatch
{
public:
    StopWatch() : m_startTime(now()) {}
    static double now() {
        return SDL_GetPerformanceCounter() / static_cast<double>(SDL_GetPerformanceFrequency());
    }
    void start() {
        m_startTime = now();
    }
    double current() const {
        return now() - m_startTime;
    }
private:
    double m_startTime;
};

} // namespace sdlc