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
    start, paused, stopped, finished, update
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
    using Callback = std::function<void(AnimatedSprite&, AnimationEvent)>;

    AnimatedSprite(SDL_Texture* texture, Callback callback = nullptr);
    AnimatedSprite(const AnimatedSprite &other);
    AnimatedSprite(AnimatedSprite &&other) noexcept;

    void render(SDL_Renderer* renderer, double deltaTime) override;
    int addFrame(const SDL_FRect& rect);
    int addFrames(const SpriteImageDistribution& dist);
    void setDuration(double time);
    void setFPS(double fps);
    void setFrame(unsigned int number);
    void play(int frameCount = -1);
    void pause();
    void stop();
    [[nodiscard]] bool playing() const {
        return m_running;
    }
    void setCallback(const Callback &cb) {
        m_callback = cb;
    }
    [[nodiscard]] bool finished() const { return !m_repeat && !m_running; }
    void setRepeat(bool state) { m_repeat = state; }
    [[nodiscard]] bool repeating() const { return m_repeat; }
    [[nodiscard]] std::size_t frameCount() const { return m_frames.size(); }
    [[nodiscard]] int currentFrame() const { return m_currentFrame; }
protected:
    virtual void handleAnimationEvent(AnimationEvent event);
private:
    int m_frameCount{0};
    bool m_running{false};
    bool m_repeat{true};
    double m_time{0};
    double m_interval{1};
    int m_currentFrame{0};
    Callback m_callback;
    std::vector<SDL_FRect> m_frames;
};

} // namespace sdlc