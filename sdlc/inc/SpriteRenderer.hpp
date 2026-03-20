#pragma once

#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>

#include <unordered_map>
#include <filesystem>

#include "Rect.hpp"

namespace sdlc
{

using SpriteDefinitions = std::unordered_map<std::string, Rect<int> >;

enum class SpritePositionOffset {
    TopLeft, TopRight, 
    BottomLeft, BottomRight, 
    Center
};

class SpriteRenderer
{
public:
    SpriteRenderer(SDL_Texture* texture);
    void setPosition(float x, float y, SpritePositionOffset offset = SpritePositionOffset::Center);
    void setPosition(const SDL_FPoint& posRef, SpritePositionOffset offset = SpritePositionOffset::Center);
    void render(SDL_Renderer* render);
    bool isOnScreen(SDL_Renderer* renderer);
    void setScaleMode(SDL_ScaleMode mode);
    void setAlphaMod(Uint8 alpha);
    void setColorMod(Uint8 r, Uint8 g, Uint8 b);
    SDL_Color getColorMod();
    void setBlendMode(SDL_BlendMode mode);
    SDL_ScaleMode getSaleMode();
    SDL_BlendMode getBlendMode();
    Uint8 getAlphaMod();
    void saveTextureState();
    void restoreTextureState();
    void setSource(const SDL_FRect& rect) { m_source = rect; }
    void setSource(float x, float y, float w, float h) {
        m_source.h = h;
        m_source.w = w;
        m_source.x = x;
        m_source.y = y;
    }
    void setDestination(const SDL_FRect& rect) {
        m_destination = rect;
    }
    void setDestination(float x, float y, float w, float h) {
        m_destination.h = h;
        m_destination.w = w;
        m_destination.x = x;
        m_destination.y = y;
    }
    [[nodiscard]] const SDL_FRect& source() const { return m_source; }
    [[nodiscard]] const SDL_FRect& destination() const { return m_destination; }
    [[nodiscard]] const SDL_FPoint& position() const { return m_position; }
    void setRotation(double angle) {
        m_rotation = angle;
        m_rotated = true;
    }
    [[nodiscard]] double rotation() const { return m_rotation; }
private:
    bool m_rotated{false};
    double m_rotation{0};
    struct {
        Uint8 alphaMod;
        SDL_Color colorMod;
        SDL_ScaleMode scaleMode;
        SDL_BlendMode blendMode;
    } m_savedState;
    SDL_Texture* m_texture;
    SDL_FRect m_source{0, 0, 0, 0};
    SDL_FRect m_destination{0, 0, 0, 0};
    SDL_FPoint m_position{0, 0};
};

bool loadSpriteDefinitions(SpriteDefinitions& def, const std::filesystem::path& path, char separator = ',');

} // namespace sdlc
