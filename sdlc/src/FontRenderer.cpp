#include <cctype>
#include <cstdlib>
#include <iostream>
#include <cassert>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "FontRenderer.hpp"

#ifdef _MSC_VER
#pragma warning (disable : 28020)
#endif // _MSC_VER

constexpr SDL_Color black = { 0, 0, 0, 255 };
constexpr SDL_Color white = { 255, 255, 255, 255 };

namespace sdl
{

FontRenderer::FontRenderer(SDL_Renderer* renderer, const char* filePath, int size, FontRenderMode mode)
    : m_renderer(renderer), m_maxCharPerLine(256)
{
    TTF_Font* ttf = TTF_OpenFont(filePath, size);
    if (!ttf) {
        std::cerr << "Error: could not load font file: " << filePath << std::endl;
        std::cerr << "SDL_GetError(): " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
    int charX = 0;
    int surfaceWith = 0;
    int surfaceHeight = TTF_GetFontHeight(ttf);
    int minx, maxx, miny, maxy, advance;
    int charHeight = TTF_GetFontHeight(ttf);
    for (int i = 0; i < sizeof(atlasString); ++i) {
        TTF_GetGlyphMetrics(ttf, atlasString[i], nullptr, nullptr, nullptr, nullptr, &advance);
        surfaceWith += advance;
    }
    SDL_Surface* fontSurface = SDL_CreateSurface(surfaceWith, surfaceHeight, SDL_PIXELFORMAT_RGBA8888);
    assert(fontSurface);
    for (int i = 0; i < sizeof(atlasString) - 1; ++i) {
        SDL_Surface* glyph;
        switch (mode) {
            case sdl::FontRenderMode::Solid:
                glyph = TTF_RenderGlyph_Solid(ttf, atlasString[i], white);
                break;
            case sdl::FontRenderMode::Blended:
                glyph = TTF_RenderGlyph_Blended(ttf, atlasString[i], white);
                break;
            case sdl::FontRenderMode::Shaded:
                glyph = TTF_RenderGlyph_Shaded(ttf, atlasString[i], white, black);
                break;
            case sdl::FontRenderMode::LCD:
                glyph = TTF_RenderGlyph_LCD(ttf, atlasString[i], white, black);
                break;
            default:
                std::cerr << "Error: invalid font render mode" << std::endl;
                exit(EXIT_FAILURE);
        }
        assert(glyph);
        TTF_GetGlyphMetrics(ttf, atlasString[i], &minx, &maxx, &miny, &maxy, &advance);
        SDL_Rect src{ 0, 0, advance, surfaceHeight };
        SDL_Rect dst{ charX, 0, src.w, src.h };
        SDL_BlitSurface(glyph, &src, fontSurface, &dst);
        SDL_DestroySurface(glyph);
        Character* ch = &m_font.characters[atlasString[i]];
        ch->x = charX;
        ch->y = 0;
        ch->advance = advance;
        ch->originX = minx;
        ch->originY = 0;
        ch->width = maxx + minx;
        ch->height = charHeight;
        charX += advance;
    }
    Uint32 key = SDL_MapSurfaceRGBA(fontSurface, black.r, black.g, black.b, black.a);
    SDL_SetSurfaceColorKey(fontSurface, true, key);
    m_fontTexture = SDL_CreateTextureFromSurface(m_renderer, fontSurface);
    assert(m_fontTexture);
    SDL_GetTextureSize(m_fontTexture, &m_textureSize.width, &m_textureSize.height);
    m_textureOwner = true;
    SDL_DestroySurface(fontSurface);
    TTF_CloseFont(ttf);
    m_font.size = size;
    m_font.characterCount = sizeof(atlasString) - 1;
    m_font.width = surfaceWith;
}

int FontRenderer::drawText(float x, float y, std::string_view text)
{
    int charCount = 0, originX = x;
    for (int i = 0; i < text.size(); ++i) {
        ++charCount;
        if ((charCount >= m_maxCharPerLine && isblank(text[i])) || (text[i] == '\n')) {
            x = originX;
            y += m_font.height;
            charCount = 0;
            continue;
        }
        Character& fc = m_font.characters[text[i]];
        SDL_FRect src{ fc.x, fc.y, fc.width, fc.height };
        SDL_FRect dst{ x, y + fc.originY, fc.width, fc.height };
        SDL_RenderTexture(m_renderer, m_fontTexture, &src, &dst);
        x += fc.advance - fc.originX;
    }
    return x;
}

void FontRenderer::drawFontTexture(float x, float y)
{
    SDL_FRect src { 0, 0, m_textureSize.width, m_textureSize.height };
    SDL_FRect dst { x, y, m_textureSize.width, m_textureSize.height };
    assert(m_fontTexture);
    assert(m_textureSize.width && m_textureSize.height);
    SDL_RenderTexture(m_renderer, m_fontTexture, &src, &dst);
}

Dimension<float> FontRenderer::measureText(std::string_view text)
{
    Dimension<float> result { 0, static_cast<float>(m_font.height) };
    for (int i = 0; i < text.size(); ++i) {
        Character& fc = m_font.characters[text[i]];
        result.width += fc.advance - fc.originX;
    }
    return result;
}

FontRenderer::~FontRenderer()
{
    SDL_DestroyTexture(m_fontTexture);
}

Font::Font()
{
    characters.fill({ 0 });
    size = width = height = characterCount = 0;
}

} // namespace sdl
