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
    BackgroundStars(sdl::AppState* state, SDL_Texture* texture);
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
        sdl::Vec2f position;
        sdl::RGB color;
        float speed;
        bool isOnScreen{false};
    };
    StarType getStarType();
private:
    double m_spawnTimer{0};
    GameAssets* m_assets;
    sdl::AppState* m_appState;
    sdl::Random m_rng;
    sdl::SpriteRenderer m_sprite;
    std::array<Star, 32> m_stars;
};

void deleteBackgroundStar(sdl::AppState* state, const std::string& name);