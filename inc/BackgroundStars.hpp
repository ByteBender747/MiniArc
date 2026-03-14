#pragma once

#include <string>
#include <SDL3/SDL_render.h>

#include "AppState.hpp"
#include "ColorConverter.hpp"
#include "MiniArc.hpp"
#include "SpriteRenderer.hpp"
#include "Vector2.hpp"
#include "Random.hpp"

class BackgroundStars {
public:
    BackgroundStars(sdlc::AppState* state, SDL_Texture* texture);
    virtual ~BackgroundStars();
private:
    void spawnStar();
    void moveAndRender();
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

void deleteBackgroundStar(sdlc::AppState* state, const std::string& name);