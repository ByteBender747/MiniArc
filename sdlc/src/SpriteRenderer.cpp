#include <cassert>

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

#include "SpriteRenderer.hpp"
#include "Rect.hpp"

namespace sdlc
{

SpriteRenderer::SpriteRenderer(SDL_Texture* texture)
    : m_savedState(), m_texture(texture)
{
    SDL_GetTextureSize(texture, &m_source.w, &m_source.h);
}

void SpriteRenderer::setPosition(float x, float y, SpritePositionOffset offset)
{
    SDL_FRect source { m_source.x, m_source.y, m_source.w * m_scale.width, m_source.h * m_scale.height };
    switch (offset) {
    case SpritePositionOffset::TopLeft:
        setDestination(x, y, source.w, source.h);
        break;
    case SpritePositionOffset::TopRight:
        setDestination(x - source.w, y, source.w, source.h);
        break;
    case SpritePositionOffset::BottomLeft:
        setDestination(x, y - source.h, source.w, source.h);
        break;
    case SpritePositionOffset::BottomRight:
        setDestination(x - source.w, y - source.h, source.w, source.h);
        break;
    case SpritePositionOffset::Center:
        setDestination(x - source.w / 2.0f, y - source.h / 2.0f, source.w, source.h);
        break;
    }
    m_position = {x, y};
}

void SpriteRenderer::setPosition(const SDL_FPoint& posRef, SpritePositionOffset offset)
{
    setPosition(posRef.x, posRef.y, offset);
}

void SpriteRenderer::setBlendMode(SDL_BlendMode mode)
{
    SDL_SetTextureBlendMode(m_texture, mode);
}

SDL_BlendMode SpriteRenderer::blendMode() const
{
    SDL_BlendMode blendMode;
    SDL_GetTextureBlendMode(m_texture, &blendMode);
    return blendMode;
}

void SpriteRenderer::setScaleMode(SDL_ScaleMode mode)
{
    SDL_SetTextureScaleMode(m_texture, mode);
}

SDL_ScaleMode SpriteRenderer::scaleMode() const
{
    SDL_ScaleMode scaleMode;
    SDL_GetTextureScaleMode(m_texture, &scaleMode);
    return scaleMode;
}

void SpriteRenderer::setAlphaMod(Uint8 alpha)
{
    SDL_SetTextureAlphaMod(m_texture, alpha);
}

Uint8 SpriteRenderer::alphaMod() const
{
    Uint8 alphaMod;
    SDL_GetTextureAlphaMod(m_texture, &alphaMod);
    return alphaMod;
}

void SpriteRenderer::setColorMod(Uint8 r, Uint8 g, Uint8 b)
{
    SDL_SetTextureColorMod(m_texture, r, g, b);
}

SDL_Color SpriteRenderer::colorMod() const
{
    SDL_Color color{0, 0, 0, 255};
    SDL_GetTextureColorMod(m_texture, &color.r, &color.g, &color.b);
    return color;
}

void SpriteRenderer::saveTextureState()
{
    m_savedState.alphaMod = alphaMod();
    m_savedState.blendMode = blendMode();
    m_savedState.scaleMode = scaleMode();
    m_savedState.colorMod = colorMod();
    m_textureStateSaved = true;
}

void SpriteRenderer::restoreTextureState()
{
    if (m_textureStateSaved) {
        setAlphaMod(m_savedState.alphaMod);
        setBlendMode(m_savedState.blendMode);
        setScaleMode(m_savedState.scaleMode);
        setColorMod(m_savedState.colorMod.r, m_savedState.colorMod.g, m_savedState.colorMod.b);
        m_textureStateSaved = false;
    } else {
        SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "Texture state has not been saved before.");
    }
}

void SpriteRenderer::setRotation(double angle)
{
    m_rotation = angle;
    m_rotated = true;
}

void SpriteRenderer::setSource(float x, float y, float w, float h)
{
    m_source.h = h;
    m_source.w = w;
    m_source.x = x;
    m_source.y = y;
}

void SpriteRenderer::setDestination(float x, float y, float w, float h)
{
    m_destination.h = h;
    m_destination.w = w;
    m_destination.x = x;
    m_destination.y = y;
}

void SpriteRenderer::render(SDL_Renderer* renderer, float deltaTime)
{
    if (m_visible && m_source.w > 0 && m_source.h > 0 && m_destination.w > 0 && m_destination.h > 0) {
        SDL_FRect destination = {m_destination.x - m_worldView.x, m_destination.y - m_worldView.y, m_destination.w, m_destination.h};
        if (m_modifier) {
            m_modifier->renderBegin(renderer, *this, deltaTime);
        }
        if (m_rotated || m_flipMode != SDL_FLIP_NONE) {
            SDL_FPoint center{destination.x + destination.w / 2, destination.y + destination.h / 2};
            SDL_RenderTextureRotated(renderer, m_texture, &m_source, &destination, m_rotation, &center, m_flipMode);
        } else {
            SDL_RenderTexture(renderer, m_texture, &m_source, &destination);
        }
        if (m_modifier) {
            m_modifier->renderEnd(renderer, *this, deltaTime);
        }
    }
}

bool SpriteRenderer::isOnScreen(SDL_Renderer* renderer) const
{
    int width, height;
    SDL_FRect destination = {m_destination.x - m_worldView.x, m_destination.y - m_worldView.y, m_destination.w, m_destination.h};
    assert(SDL_GetCurrentRenderOutputSize(renderer, &width, &height));
    Rect<float> screenRect(0, 0, width, height);
    Rect<float> source(destination);
    return screenRect.intersects(source);
}

} // namespace sdlc
