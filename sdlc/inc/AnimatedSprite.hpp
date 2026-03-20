#pragma once

#include "SpriteRenderer.hpp"
#include <SDL3/SDL_rect.h>

#include <vector>
#include <cstdint>
#include <functional>

namespace sdlc
{

enum class AnimationEvent
{
    start, paused, stopped, finished
};

struct SpriteImageDistribution {
    int startX;
    int startY;
    int spriteWidth;
    int spriteHeight;
    int marginX;
    int marginY;
    int spritesPerLine;
    int spriteCount;
};

class AnimatedSprite : public SpriteRenderer
{
public:
    using AnimationCallback = std::function<void(AnimatedSprite&, AnimationEvent)>;

    AnimatedSprite(SDL_Texture* texture);
    void update(double deltaTime);
    void addFrame(const SDL_FRect& rect);
    void addFrames(const SpriteImageDistribution& dist);
    void setDuration(double time);
    void setFPS(double fps);
    void setFrame(unsigned int number);
    void play();
    void pause();
    void stop();
    bool isPlaying() const {
        return m_running;
    }
    void setCallback(const AnimationCallback &cb) {
        m_callback = cb;
    }
    bool isFinished() const { return !m_repeat && !m_running; }
    void setRepeat(bool state) { m_repeat = state; }
    bool repeating() const { return m_repeat; }
private:
    bool m_running{false};
    bool m_repeat{true};
    double m_time{0};
    double m_interval{1};
    uint32_t m_currentFrame{0};
    AnimationCallback m_callback;
    std::vector<SDL_FRect> m_frames;
};

} // namespace sdlc