#include <cassert>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

#include "SpriteRenderer.hpp"
#include "Rect.hpp"

namespace sdlc
{

SpriteRenderer::SpriteRenderer(SDL_Texture* texture)
    : m_texture(texture), m_rotation(0), m_rotated(false)
{
}

void SpriteRenderer::setPosition(float x, float y, SpritePositionOffset origin)
{
    switch (origin) {
    case sdlc::SpritePositionOffset::TopLeft:
        setDestination(x, y, m_source.w, m_source.h);
        break;
    case sdlc::SpritePositionOffset::TopRight:
        setDestination(x - m_source.w, y, m_source.w, m_source.h);
        break;
    case sdlc::SpritePositionOffset::BottomLeft:
        setDestination(x, y - m_source.h, m_source.w, m_source.h);
        break;
    case sdlc::SpritePositionOffset::BottomRight:
        setDestination(x - m_source.w, y - m_source.h, m_source.w, m_source.h);
        break;
    case sdlc::SpritePositionOffset::Center:
        setDestination(x - m_source.w / 2.0f, y - m_source.h / 2.0f, m_source.w, m_source.h);
        break;
    }
    m_position = {x, y};
}

void SpriteRenderer::setBlendMode(SDL_BlendMode mode)
{
    SDL_SetTextureBlendMode(m_texture, mode);
}

SDL_BlendMode SpriteRenderer::getBlendMode()
{
    SDL_BlendMode blendMode;
    SDL_GetTextureBlendMode(m_texture, &blendMode);
    return blendMode;
}

void SpriteRenderer::setScaleMode(SDL_ScaleMode mode)
{
    SDL_SetTextureScaleMode(m_texture, mode);
}

SDL_ScaleMode SpriteRenderer::getSaleMode()
{
    SDL_ScaleMode scaleMode;
    SDL_GetTextureScaleMode(m_texture, &scaleMode);
    return scaleMode;
}

void SpriteRenderer::setAlphaMod(Uint8 alpha)
{
    SDL_SetTextureAlphaMod(m_texture, alpha);
}

Uint8 SpriteRenderer::getAlphaMod()
{
    Uint8 alphaMod;
    SDL_GetTextureAlphaMod(m_texture, &alphaMod);
    return alphaMod;
}

void SpriteRenderer::setColorMod(Uint8 r, Uint8 g, Uint8 b)
{
    SDL_SetTextureColorMod(m_texture, r, g, b);
}

SDL_Color SpriteRenderer::getColorMod()
{
    SDL_Color color;
    SDL_GetTextureColorMod(m_texture, &color.r, &color.g, &color.b);
    return color;
}

void SpriteRenderer::saveTextureState()
{
    m_savedState.alphaMod = getAlphaMod();
    m_savedState.blendMode = getBlendMode();
    m_savedState.scaleMode = getSaleMode();
    m_savedState.colorMod = getColorMod();
}

void SpriteRenderer::restoreTextureState()
{
    setAlphaMod(m_savedState.alphaMod);
    setBlendMode(m_savedState.blendMode);
    setScaleMode(m_savedState.scaleMode);
    setColorMod(m_savedState.colorMod.r, m_savedState.colorMod.g, m_savedState.colorMod.b);
}

void SpriteRenderer::render(SDL_Renderer* renderer)
{
    if (m_rotated) {
        SDL_FPoint center{m_destination.x + m_destination.w / 2, m_destination.y + m_destination.h / 2};
        SDL_RenderTextureRotated(renderer, m_texture, &m_source, &m_destination, m_rotation, &center, SDL_FLIP_NONE);
    } else {
        SDL_RenderTexture(renderer, m_texture, &m_source, &m_destination);
    }
}

bool SpriteRenderer::isOnScreen(SDL_Renderer* renderer)
{
    int width, height;
    assert(SDL_GetCurrentRenderOutputSize(renderer, &width, &height));
    Rect<float> screenRect(0, 0, width, height);
    Rect<float> source(m_source);
    return screenRect.intersects(source);
}

template <typename value_type>
value_type ParseValue(const std::string& sval, bool& error)
{
    value_type value;
    std::istringstream ss(sval);
    ss >> value;
    if (ss.fail()) {
        std::cerr << "Error: could not parse '" << sval << "'" << std::endl;
        error = true;
    }
    return value;
}

bool loadSpriteDefinitions(SpriteDefinitions& def, const std::filesystem::path& path, char separator)
{
    std::string line;
    std::ifstream stream(path, std::fstream::in);
    if (!stream.is_open()) {
        return false;
    }
    while (getline(stream, line)) {
        std::istringstream iss(line);
        std::string name, sx, sy, sw, sh;
        getline(iss, name, separator);
        getline(iss, sx, separator);
        getline(iss, sy, separator);
        getline(iss, sw, separator);
        getline(iss, sh, separator);
        bool error = false;
        auto x = ParseValue<int>(sx, error);
        auto y = ParseValue<int>(sy, error);
        auto w = ParseValue<int>(sw, error);
        auto h = ParseValue<int>(sh, error);
        if (!error) {
            Rect<int> r(x, y, w, h);
            def[name] = r;
        }
    }
    return true;
}

} // namespace sdlc
