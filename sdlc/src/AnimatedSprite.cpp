#include "AnimatedSprite.hpp"
#include "SpriteRenderer.hpp"

namespace sdlc
{

AnimatedSprite::AnimatedSprite(SDL_Texture* texture)
    : SpriteRenderer(texture)
    , m_time(0), m_interval(1), m_currentFrame(0), m_running(true)
    , m_animationFinished(nullptr)
{
}

void AnimatedSprite::update(double deltaTime)
{
    if (m_running) {
        m_time += deltaTime;
        if (m_time >= m_interval) {
            m_time = 0;
            m_currentFrame = (m_currentFrame + 1) % m_frames.size();
            setSource(m_frames[m_currentFrame]);
            if (!m_currentFrame && m_animationFinished) {
                m_animationFinished(*this);
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

void AnimatedSprite::setFrame(int number)
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