#include <ios>
#include <cassert>
#include <format>

#include "UILayer.hpp"
#include "AppState.hpp"
#include "FontRenderer.hpp"
#include "HealthBar.hpp"
#include "MiniArc.hpp"
#include "Utility.hpp"
#include "PlayerShip.hpp"
#include "Rect.hpp"
#include "SpriteRenderer.hpp"

#include <SDL3_image/SDL_image.h>

UILayer::UILayer(sdlc::AppState* appState)
    : m_appState(appState)
{
    m_font = new sdlc::FontRenderer(
        m_appState->renderer,
        sdlc::ResolveRelativeToExe("../Assets/Gameplay.ttf").c_str(), 
        32, sdlc::FontRenderMode::Solid);
    m_appState->iterateHandler[uiZIndex] = [this](sdlc::AppState* appState) {
        renderScoreValue();
        renderHealthBar();
        renderShipCount();
        renderModTime();
        if (isGameOver()) {
            sdlc::SpriteRenderer sr(m_gameOverImage);
            sr.setPosition(RENDER_LOGICAL_WIDTH / 2., RENDER_LOGICAL_HEIGHT / 2.);
            sr.render(m_appState->renderer);
            if (!m_gameOverSfxFlag) {
                m_appState->audio.stream[strmPlayerEffects].setGain(.35f);
                m_appState->audio.stream[strmPlayerEffects].put(m_assets->gameOver);
                m_gameOverSfxFlag = true;
            }
            m_appState->properties["gameOver"].boolean = true;
        }
    };
    m_assets = static_cast<GameAssets*>(m_appState->properties["assets"].pointer);
    m_startImage = sdlc::LoadTexture(m_appState->renderer, sdlc::ResolveRelativeToExe("../Assets/start.png").c_str());
    m_gameOverImage = sdlc::LoadTexture(m_appState->renderer, sdlc::ResolveRelativeToExe("../Assets/game_over.png").c_str());
    m_font->setScale(0.25);
}

bool UILayer::isGameOver()
{
    PlayerShip* player = static_cast<PlayerShip*>(m_appState->properties["player"].pointer);
    return !player->isAlive() && !player->isSpawning() && m_appState->properties["playerShips"].integer <= 0;
}

UILayer::~UILayer()
{
    m_appState->iterateHandler[uiZIndex] = nullptr;
    delete m_font;
}

void UILayer::renderShipCount()
{
    int shipCount = static_cast<int>(m_appState->properties["playerShips"].integer);
    sdlc::SpriteRenderer sprite(m_assets->spriteTexture);
    sprite.setSource(m_assets->sprites["PlayerSmall"]);
    m_font->setTextColor(sdlc::RGBA(255, 255, 255, 255));
    m_font->renderText(RENDER_LOGICAL_WIDTH - 20, RENDER_LOGICAL_HEIGHT - 15, std::format("x{}", shipCount));
    sprite.setPosition(RENDER_LOGICAL_WIDTH - 33, RENDER_LOGICAL_HEIGHT - 14, sdlc::SpritePositionOffset::TopLeft);
    sprite.render(m_appState->renderer);
}

void UILayer::renderModTime()
{
    double modTime = getPlayer()->getWeaponPowerUpTime();
    if (modTime > 0) {
        std::string numberStr = std::format("{:.1f}", modTime);
        auto dim = m_font->measureText(numberStr);
        float xpos = static_cast<float>(RENDER_LOGICAL_WIDTH) / 2 - dim.width / 2;
        m_font->setTextColor(sdlc::RGBA(0, 0, 255, 255));
        m_font->renderText(xpos, 5, numberStr);
    }
}

void UILayer::renderHealthBar()
{
    HealthBar hbar(m_appState, sdlc::Rect<float>(8, 5, 40, 5));
    hbar.setRange(playerInitialHitPoints);
    hbar.setValue(getPlayer()->getCurrentHitPointe());
    hbar.render();
}

PlayerShip* UILayer::getPlayer()
{
    PlayerShip* player = static_cast<PlayerShip*>(m_appState->properties["player"].pointer);
    assert(player);
    return player;
}

void UILayer::renderScoreValue()
{
    int64_t score = m_appState->properties["score"].integer;
    std::string numberStr = std::format("{:07}", score);
    sdlc::Dimension<float> dim = m_font->measureText(numberStr);
    m_font->setTextColor(sdlc::RGBA(255, 255, 255, 255));
    m_font->renderText(RENDER_LOGICAL_WIDTH - dim.width - 10, 5, numberStr);
}

void deleteUI(sdlc::AppState* state, const std::string& name)
{
    auto* ptr = static_cast<UILayer*>(state->properties[name].pointer);
    if (ptr) {
        delete ptr;
        state->properties[name].pointer = nullptr;
    }
}