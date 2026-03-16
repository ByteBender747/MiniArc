#pragma once

#include <array>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include "ColorConverter.hpp"
#include "Dimension.hpp"

namespace sdlc
{

constexpr char atlasString[] =
    "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ ";

struct Character {
    float x, y, width, height, originX, originY, advance;
};

struct Font {
    int size;
    int width, height, characterCount;
    std::array<Character, 256> characters;
    Font();
};

enum class FontRenderMode {
    Solid, Blended, LCD, Shaded
};

class FontRenderer
{
public:
    FontRenderer(SDL_Renderer* renderer, const char* filePath, int size, FontRenderMode mode);
    int renderText(float x, float y, std::string_view text);
    void renderFontTexture(float x, float y);
    Dimension<float> measureText(std::string_view text);
    int fontHeight() const {
        return m_font.height;
    }
    int fontWidth() const {
        return m_font.width;
    }
    int size() const {
        return m_font.size;
    }
    int charPerLine() const {
        return m_maxCharPerLine;
    }
    void setCharsPerLine(int value) {
        m_maxCharPerLine = value;
    }
    void setTextColor(const RGBA& color) {
        m_textColor = color;
    }
    const RGBA& getTextColor() const {
        return m_textColor;
    }
    ~FontRenderer();

protected:
    void preserveLogicalPresentationMode();
    void restoreLogicalPresentationMode();
    void disableLogicalPresentationMode();
    void renderText(const SDL_FRect& src, const SDL_FRect& dst);

private:
    Font m_font;
    int m_maxCharPerLine;
    bool m_textureOwner;
    RGBA m_textColor{255, 255, 255, 255};
    Dimension<int> m_logicalSize;
    Dimension<float> m_textureSize;
    SDL_RendererLogicalPresentation m_logicalMode;
    SDL_Renderer* m_renderer;
    SDL_Texture* m_fontTexture;
};

using FontMap = std::unordered_map<std::string, std::shared_ptr<FontRenderer>>;

} // namespace sdlc
