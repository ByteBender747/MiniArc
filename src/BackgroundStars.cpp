#include "BackgroundStars.hpp"
#include "ColorConverter.hpp"
#include "GameConfig.hpp"
#include "MathFunc.hpp"
#include "MiniArc.hpp"

constexpr float starSpeed[] = { 120, 80, 45 };
constexpr double starSpawnInterval = 0.1;

BackgroundStars::BackgroundStars(MiniArcGame* game)
    : AppLayer("BackgroundStars", backgroundZIndex)
    , m_sprite(game->assets.spriteTexture)
    , m_appState(game->appState)
    , m_assets(&game->assets)
{
}

void BackgroundStars::update(float deltaTime)
{
    m_spawnTimer += deltaTime;
    if (m_spawnTimer > starSpawnInterval) {
        m_spawnTimer = 0;
        spawnStar();
    }
    for (auto& star : m_stars) {
        if (star.isOnScreen) {
            star.position += sdlc::Vec2f(0, star.speed * deltaTime);
            if (star.position.y > RENDER_LOGICAL_HEIGHT + 7) {
                star.isOnScreen = false;
            }
        }
    }
}

void BackgroundStars::render(SDL_Renderer* renderer)
{
    for (auto& star : m_stars) {
        if (star.isOnScreen) {
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
            m_sprite.setPosition(star.position.x, star.position.y, sdlc::SpritePositionOffset::Center);
            m_sprite.render(renderer);
            m_sprite.restoreTextureState();
        }
    }
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
            star.color = sdlc::hslToRgb(hsl);
            return;
        }
    }
}
