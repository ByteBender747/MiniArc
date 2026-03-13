#include "BackgroundStars.hpp"
#include "AppState.hpp"
#include "ColorConverter.hpp"
#include "MiniArc.hpp"
#include "SpriteRenderer.hpp"
#include "Vector2.hpp"
#include "MathFunc.hpp"

constexpr int starsZIndex = 0;
constexpr float starSpeed[] = { 120, 80, 45 };
constexpr double spawnInterval = 0.3;

BackgroundStars::BackgroundStars(sdl::AppState* state, SDL_Texture* texture)
    : m_sprite(texture), m_appState(state)
{
    m_assets = static_cast<GameAssets*>(state->properties["assets"].pointer);
    m_appState->iterateHandler[starsZIndex] = [this](sdl::AppState* appState) {
        m_spawnTimer += appState->deltaTime;
        if (m_spawnTimer > spawnInterval) {
            m_spawnTimer = 0;
            spawnStar();
        }
        moveAndRender();
    };
}

BackgroundStars::~BackgroundStars()
{
    m_appState->iterateHandler[starsZIndex] = nullptr;
}

void BackgroundStars::spawnStar()
{
    for (auto& star : m_stars) {
        if (!star.isOnScreen) {
            star.position = sdl::Vec2f(m_rng.next() % RENDER_LOGICAL_WIDTH, -7);
            star.isOnScreen = true;
            star.type = static_cast<StarType>(m_rng.next() % 3);
            sdl::HSL hsl { 0, 1, .5 };
            switch (star.type) {
                case StarType::Small:
                    hsl.h = sdl::lerp(170, 230, m_rng.uniform());
                    break;
                case StarType::Middle:
                    hsl.h = sdl::lerp(270, 330, m_rng.uniform());             
                    break;
                case StarType::Large:
                    hsl.h = sdl::lerp(0, 60, m_rng.uniform());                 
                    break;
            }
            hsl.l = sdl::lerp(.1, .5, m_rng.uniform());
            star.speed = starSpeed[static_cast<int>(star.type)];
            star.color = sdl::ColorConverter::hslToRgb(hsl);
            return;
        }
    }
}

void BackgroundStars::moveAndRender()
{
    for (auto& star : m_stars) {
        if (star.isOnScreen) {
            star.position += sdl::Vec2f(0, star.speed * m_appState->deltaTime);
            if (star.position.y > RENDER_LOGICAL_HEIGHT + 7) {
                star.isOnScreen = false;
            }
            switch (star.type) {
                case StarType::Small:
                    m_sprite.setSource(m_assets->sprites["StarSmall"]);
                    break;
                case StarType::Middle:
                    m_sprite.setSource(m_assets->sprites["StarMedium"]);
                    break;
                case StarType::Large:
                    m_sprite.setSource(m_assets->sprites["StarLarge"]);
                    break;
            }
            m_sprite.saveTextureState();
            m_sprite.setColorMod(star.color.r, star.color.g, star.color.b);
            m_sprite.setPosition(star.position.x, star.position.y, sdl::SpriteOrigin::Center);
            m_sprite.render(m_appState->renderer);
            m_sprite.restoreTextureState();
        }
    }
}