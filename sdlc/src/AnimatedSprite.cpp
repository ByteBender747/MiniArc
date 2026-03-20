#include "AnimatedSprite.hpp"
#include "SpriteRenderer.hpp"

namespace sdlc
{

AnimatedSprite::AnimatedSprite(SDL_Texture* texture)
    : SpriteRenderer(texture)
    , m_callback(nullptr)
{
}

void AnimatedSprite::stop()
{
    m_running = false;
    m_currentFrame = 0;
    m_time = 0;
    setSource(0, 0, 0, 0);
    if (m_callback) m_callback(*this, AnimationEvent::stopped);
}

void AnimatedSprite::play()
{
    if (!m_running) {
        m_running = true;
        if (m_callback) m_callback(*this, AnimationEvent::start);
    }
}

void AnimatedSprite::pause()
{
    if (m_running) {
        m_running = false;
        if (m_callback) m_callback(*this, AnimationEvent::paused);
    }
}

void AnimatedSprite::update(double deltaTime)
{
    if (m_running) {
        m_time += deltaTime;
        if (m_time >= m_interval) {
            m_time = 0;
            m_currentFrame = (m_currentFrame + 1) % m_frames.size();
            setSource(m_frames[m_currentFrame]);
            if (!m_currentFrame) {
                if (m_callback) m_callback(*this, AnimationEvent::finished);
                if (!m_repeat) stop();
            }
        }
    }
}

void AnimatedSprite::addFrame(const SDL_FRect& rect)
{
    m_frames.emplace_back(rect);
    m_currentFrame = 0;
    setSource(m_frames[m_currentFrame]);
}

void AnimatedSprite::setDuration(double time)
{
    m_interval = time / static_cast<double>(m_frames.size());
}

void AnimatedSprite::setFPS(double fps)
{
    m_interval = 1.0 / static_cast<double>(fps);
}

void AnimatedSprite::setFrame(unsigned int number)
{
    if (number < m_frames.size()) {
        m_currentFrame = number;
        setSource(m_frames[m_currentFrame]);
    }
}

void AnimatedSprite::addFrames(const SpriteImageDistribution& dist)
{
    int row = 0, col = 0;
    for (int i = 0; i < dist.spriteCount; ++i) {
        float x = dist.startX + ((dist.spriteWidth + dist.marginX) * col);
        float y = dist.startY + ((dist.spriteHeight + dist.marginY) * row);
        addFrame({ x, y, static_cast<float>(dist.spriteWidth), static_cast<float>(dist.spriteHeight) });
        if (++col >= dist.spritesPerLine && dist.spritesPerLine > 0) {
            col = 0;
            ++row;
        }
    }
}

} // namespace sdlc