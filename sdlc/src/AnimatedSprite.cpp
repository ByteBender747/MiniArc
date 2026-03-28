#include "AnimatedSprite.hpp"

#include <utility>
#include "SpriteRenderer.hpp"

namespace sdlc
{

AnimatedSprite::AnimatedSprite(const AnimatedSprite &other)
    : SpriteRenderer(other),
      m_frameCount(other.m_frameCount),
      m_running(other.m_running),
      m_repeat(other.m_repeat),
      m_time(other.m_time),
      m_interval(other.m_interval),
      m_currentFrame(other.m_currentFrame),
      m_callback(other.m_callback)
{
    m_frames.reserve(other.m_frames.size());
    std::copy(other.m_frames.begin(), other.m_frames.end(), m_frames.begin());
}

AnimatedSprite::AnimatedSprite(AnimatedSprite &&other) noexcept
    : SpriteRenderer(other),
      m_frameCount(other.m_frameCount),
      m_running(other.m_running),
      m_repeat(other.m_repeat),
      m_time(other.m_time),
      m_interval(other.m_interval),
      m_currentFrame(other.m_currentFrame),
      m_callback(std::move(other.m_callback)),
      m_frames(std::move(other.m_frames))
{
}

AnimatedSprite::AnimatedSprite(SDL_Texture *texture, Callback callback)
    : SpriteRenderer(texture)
    , m_callback(std::move(callback))
{
}

void AnimatedSprite::stop()
{
    m_running = false;
    m_currentFrame = 0;
    m_time = 0;
    setSource(0, 0, 0, 0);
    handleAnimationEvent(AnimationEvent::stopped);
}

void AnimatedSprite::play(int frameCount)
{
    if (!m_running) {
        m_running = true;
        if (frameCount > 0) {
            m_frameCount = frameCount;
            if (m_frameCount > m_frames.size()) {
                m_frameCount = static_cast<int>(m_frames.size());
            }
        } else {
            m_frameCount = static_cast<int>(m_frames.size());
        }
        handleAnimationEvent(AnimationEvent::start);
    }
}

void AnimatedSprite::pause()
{
    if (m_running) {
        m_running = false;
        handleAnimationEvent(AnimationEvent::paused);
    }
}

void AnimatedSprite::render(SDL_Renderer *renderer, float deltaTime)
{
    if (m_running) {
        m_time += deltaTime;
        if (m_time >= m_interval) {
            m_time = 0;
            m_currentFrame = (m_currentFrame + 1) % m_frameCount;
            setSource(m_frames[m_currentFrame]);
            handleAnimationEvent(AnimationEvent::update);
            if (!m_currentFrame) {
                handleAnimationEvent(AnimationEvent::finished);
                if (!m_repeat) stop();
            }
        }
    }
    SpriteRenderer::render(renderer, deltaTime);
}

void AnimatedSprite::handleAnimationEvent(AnimationEvent event)
{
    if (m_callback) {
        m_callback(*this, event);
    }
}

int AnimatedSprite::addFrame(const SDL_FRect &rect)
{
    m_frames.emplace_back(rect);
    m_currentFrame = 0;
    m_frameCount = static_cast<int>(m_frames.size());
    setSource(m_frames[m_currentFrame]);
    return m_frameCount - 1;
}

void AnimatedSprite::setDuration(float time)
{
    m_interval = time / static_cast<float>(m_frames.size());
}

void AnimatedSprite::setFPS(float fps)
{
    m_interval = 1.0 / static_cast<float>(fps);
}

void AnimatedSprite::setFrame(unsigned int number)
{
    if (number < m_frames.size()) {
        m_currentFrame = number;
        setSource(m_frames[m_currentFrame]);
    }
}

int AnimatedSprite::addFrames(const SpriteImageDistribution &dist)
{
    int row = 0, col = 0;
    for (int i = 0; i < dist.spriteCount; ++i) {
        float x = dist.startX + ((dist.spriteWidth + dist.marginX) * col);
        float y = dist.startY + ((dist.spriteHeight + dist.marginY) * row);
        addFrame({x, y, static_cast<float>(dist.spriteWidth), static_cast<float>(dist.spriteHeight)});
        if (++col >= dist.spritesPerLine && dist.spritesPerLine > 0) {
            col = 0;
            ++row;
        }
    }
    return m_frameCount - 1;
}

} // namespace sdlc
