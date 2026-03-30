#pragma once

#include <array>
#include <SDL3/SDL_render.h>

#include "AppLayer.hpp"
#include "ColorConverter.hpp"
#include "SpriteRenderer.hpp"
#include "Vector2.hpp"
#include "Random.hpp"

struct MiniArcGame;
struct GameAssets;

class BackgroundStars : public sdlc::AppLayer {
public:
    BackgroundStars(MiniArcGame* game);
    void render(SDL_Renderer* renderer) override;
    void update(float deltaTime) override;
private:
    void spawnStar();
private:
    enum class StarType {
        Small, Middle, Large
    };
    struct Star {
        StarType type;
        sdlc::Vec2f position;
        sdlc::RGB color;
        float speed;
        bool isOnScreen{false};
    };
    StarType getStarType();
private:
    double m_spawnTimer{0};
    GameAssets* m_assets;
    sdlc::AppState* m_appState;
    sdlc::Random m_rng;
    sdlc::SpriteRenderer m_sprite;
    std::array<Star, 32> m_stars;
};