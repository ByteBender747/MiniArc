#pragma once

#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>

#include <string>
#include <unordered_map>

#include "Dimension.hpp"
#include "Rect.hpp"

namespace sdlc
{

using SpriteDefinitions = std::unordered_map<std::string, Rect<int> >;

enum class SpritePositionOffset {
    TopLeft, TopRight, 
    BottomLeft, BottomRight, 
    Center
};

class SpriteRenderer;

class SpriteRenderModifier
{
public:
    virtual ~SpriteRenderModifier() = default;
    virtual void renderBegin(SDL_Renderer* renderer, SpriteRenderer& spriteRenderer, float deltaTime) = 0;
    virtual void renderEnd(SDL_Renderer* renderer, SpriteRenderer& spriteRenderer, float deltaTime) = 0;
};

class SpriteRenderer
{
public:
    SpriteRenderer(SDL_Texture* texture);
    virtual ~SpriteRenderer() = default;
    void setPosition(float x, float y, SpritePositionOffset offset = SpritePositionOffset::Center);
    void setPosition(const SDL_FPoint& posRef, SpritePositionOffset offset = SpritePositionOffset::Center);
    virtual void render(SDL_Renderer* renderer, float deltaTime);
    void render(SDL_Renderer* renderer) { render(renderer, 0); }
    bool isOnScreen(SDL_Renderer* renderer) const;
    void setScaleMode(SDL_ScaleMode mode);
    void setAlphaMod(Uint8 alpha);
    void setColorMod(Uint8 r, Uint8 g, Uint8 b);
    void setBlendMode(SDL_BlendMode mode);
    [[nodiscard]] SDL_Color colorMod() const;
    [[nodiscard]] SDL_ScaleMode scaleMode() const;
    [[nodiscard]] SDL_BlendMode blendMode() const;
    [[nodiscard]] Uint8 alphaMod() const;
    void saveTextureState();
    void restoreTextureState();
    void setRotation(double angle);
    void setSource(float x, float y, float w, float h);
    void setDestination(float x, float y, float w, float h);
    void setSource(const SDL_FRect& rect) { m_source = rect; }
    void setDestination(const SDL_FRect& rect) { m_destination = rect; }
    void setModifier(SpriteRenderModifier *modifier) { m_modifier = modifier; }
    void hide() { m_visible = false; }
    void show() { m_visible = true; }
    void setScale(float x, float y) { m_scale.width = x; m_scale.height = y; }
    void setWorldView(float x, float y) { m_worldView.x = x; m_worldView.y = y; }
    void setFlipMode(SDL_FlipMode flipMode) { m_flipMode = flipMode; }
    [[nodiscard]] bool visible() const { return m_visible; }
    [[nodiscard]] SpriteRenderModifier * modifier() const { return m_modifier; }
    [[nodiscard]] const SDL_FRect& source() const { return m_source; }
    [[nodiscard]] const SDL_FRect& destination() const { return m_destination; }
    [[nodiscard]] const SDL_FPoint& position() const { return m_position; }
    [[nodiscard]] double rotation() const { return m_rotation; }
    [[nodiscard]] const Dimension<float> & scale() const { return m_scale; }
    [[nodiscard]] const Point2D<float>& worldView() const { return m_worldView; }
    [[nodiscard]] SDL_FlipMode flipMode() const { return m_flipMode; }
private:
    bool m_visible{true};
    bool m_rotated{false};
    double m_rotation{0};
    struct {
        Uint8 alphaMod;
        SDL_Color colorMod;
        SDL_ScaleMode scaleMode;
        SDL_BlendMode blendMode;
    } m_savedState;
    Point2D<float> m_worldView;
    SpriteRenderModifier* m_modifier{nullptr};
    Dimension<float> m_scale{1, 1};
    bool m_textureStateSaved{false};
    SDL_FlipMode m_flipMode{SDL_FLIP_NONE};
    SDL_Texture* m_texture;
    SDL_FRect m_source{0, 0, 0, 0};
    SDL_FRect m_destination{0, 0, 0, 0};
    SDL_FPoint m_position{0, 0};
};

} // namespace sdlc
