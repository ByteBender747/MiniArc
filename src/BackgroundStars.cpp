#include "BackgroundStars.hpp"
#include "AppState.hpp"
#include "ColorConverter.hpp"
#include "MiniArc.hpp"
#include "SpriteRenderer.hpp"
#include "Vector2.hpp"
#include "MathFunc.hpp"

constexpr int starsZIndex = 0;
constexpr float starSpeed[] = { 120, 80, 45 };
constexpr double spawnInterval = 0.1;

BackgroundStars::BackgroundStars(sdlc::AppState* state, SDL_Texture* texture)
    : m_sprite(texture), m_appState(state)
{
    m_assets = static_cast<GameAssets*>(state->properties["assets"].pointer);
    m_appState->iterateHandler[starsZIndex] = [this](sdlc::AppState* appState) {
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

BackgroundStars::StarType BackgroundStars::getStarType()
{
    StarType result = StarType::Small;
    if (m_rng.chance(.2)) {
        result = StarType::Large;
    } else if (m_rng.chance(.4)) {
        result = StarType::Middle;
    }
    return result;
}

void BackgroundStars::spawnStar()
{
    for (auto& star : m_stars) {
        if (!star.isOnScreen) {
            star.position = sdlc::Vec2f(m_rng.next() % RENDER_LOGICAL_WIDTH, -7);
            star.isOnScreen = true;
            star.type = getStarType();
            sdlc::HSL hsl { 0, 1, .5 };
            switch (star.type) {
                case StarType::Small:
                    hsl.h = sdlc::lerp(170, 230, m_rng.uniform());
                    break;
                case StarType::Middle:
                    hsl.h = sdlc::lerp(270, 330, m_rng.uniform());             
                    break;
                case StarType::Large:
                    hsl.h = sdlc::lerp(0, 60, m_rng.uniform());                 
                    break;
            }
            hsl.l = sdlc::lerp(.1, .5, m_rng.uniform());
            star.speed = starSpeed[static_cast<int>(star.type)];
            star.color = sdlc::ColorConverter::hslToRgb(hsl);
            return;
        }
    }
}

void BackgroundStars::moveAndRender()
{
    for (auto& star : m_stars) {
        if (star.isOnScreen) {
            star.position += sdlc::Vec2f(0, star.speed * m_appState->deltaTime);
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
            m_sprite.setScaleMode(SDL_SCALEMODE_LINEAR);
            m_sprite.setColorMod(star.color.r, star.color.g, star.color.b);
            m_sprite.setPosition(star.position.x, star.position.y, sdlc::SpriteOrigin::Center);
            m_sprite.render(m_appState->renderer);
            m_sprite.restoreTextureState();
        }
    }
}

void deleteBackgroundStar(sdlc::AppState* state, const std::string& name)
{
    BackgroundStars* ptr = static_cast<BackgroundStars*>(state->properties[name].pointer);
    if (ptr) {
        delete ptr;
        state->properties[name].pointer = nullptr;
    }
}