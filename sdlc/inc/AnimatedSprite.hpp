#pragma once

#include "SpriteRenderer.hpp"
#include <SDL3/SDL_rect.h>

#include <vector>
#include <cstdint>
#include <functional>

namespace sdlc
{

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
    using FinishedCallback = std::function<void(AnimatedSprite& ref)>;

    AnimatedSprite(SDL_Texture* texture);
    void update(double deltaTime);
    void addFrame(const SDL_FRect& rect);
    void addFrames(const SpriteImageDistribution& dist);
    void setDuration(double time);
    void setFPS(int fps);
    void play() {
        m_running = true;
    }
    void pause() {
        m_running = false;
    }
    void stop() {
        m_running = false;
        m_currentFrame = 0;
        m_time = 0;
    }
    bool isPlaying() const {
        return m_running;
    }
    void animationFinished(FinishedCallback cb) {
        m_animationFinished = cb;
    }
private:
    bool m_running;
    double m_time;
    double m_interval;
    uint32_t m_currentFrame;
    FinishedCallback m_animationFinished;
    std::vector<SDL_FRect> m_frames;
};

} // namespace sdlc