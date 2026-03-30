#include <cassert>
#include <format>

#include "UILayer.hpp"
#include "AppState.hpp"
#include "FontRenderer.hpp"
#include "HealthBar.hpp"
#include "MiniArc.hpp"
#include "PlayerShip.hpp"
#include "Rect.hpp"
#include "SpriteRenderer.hpp"
#include "Utility.hpp"

NameInput::NameInput(sdlc::AppState *appState, sdlc::FontRenderer *font)
    : m_textInput(appState->renderer, font)
    , m_appState(appState)
    , m_font(font)
{
    m_textInput.setCallback([this](std::string_view text, bool accepted) {
        if (accepted) {
            m_name = text;
        }
        m_inputFinished = true;
    });
}

void NameInput::render()
{
    float oldScale = m_font->scale();
    std::string caption = "Your Name: ";
    sdlc::Point2D<float> pos(RENDER_LOGICAL_WIDTH / 2 - 10, RENDER_LOGICAL_HEIGHT / 2 + 16);
    m_font->setScale(0.15);
    m_font->renderText(pos.x, pos.y, caption);
    m_textInput.render(pos.x + m_font->measureText(caption).width, pos.y);
    m_font->setScale(oldScale);
}

void NameInput::handleEvent(SDL_Event& event)
{
    m_textInput.handleEvent(event);
}

UILayer::UILayer(MiniArcGame* game)
    : AppLayer("UILayer", uiZIndex)
    , m_appState(game->appState)
    , m_assets(&game->assets)
{
    m_font = new sdlc::FontRenderer(
        m_appState->renderer,
        sdlc::ResolveRelativeToExe("../Assets/Gameplay.ttf").c_str(),
        32, sdlc::FontRenderMode::Solid);
    m_startImage = sdlc::LoadTexture(m_appState->renderer, sdlc::ResolveRelativeToExe("../Assets/start.png").c_str());
    m_gameOverImage = sdlc::LoadTexture(m_appState->renderer, sdlc::ResolveRelativeToExe("../Assets/game_over.png").c_str());
    m_font->setScale(0.25);
}

UILayer::~UILayer()
{
    delete m_nameInput;
    delete m_font;
}

void UILayer::update(float deltaTime)
{
}

void UILayer::render(SDL_Renderer* renderer)
{
    renderScoreValue();
    renderHealthBar();
    renderShipCount();
    renderModTime();
    if (isGameOver()) {
        sdlc::SpriteRenderer sr(m_gameOverImage);
        sr.setPosition(RENDER_LOGICAL_WIDTH / 2., RENDER_LOGICAL_HEIGHT / 2.);
        sr.render(renderer);
        if (!m_gameOverSfxFlag) {
            m_appState->audio.stream[strmPlayerEffects].setGain(.35f);
            m_appState->audio.stream[strmPlayerEffects].put(m_assets->gameOver);
            m_gameOverSfxFlag = true;
        }
        m_appState->properties["gameOver"].boolean = true;
        if (!m_nameInput && !m_hiscoreDone) {
            sdlc::TextInput::startTextInput(m_appState->window);
            m_nameInput = new NameInput(m_appState, m_font);
        } else if (m_nameInput && m_nameInput->hasFinished()) {
            sdlc::TextInput::stopTextInput(m_appState->window);
            m_hiscoreDone = true;
            delete m_nameInput;
            m_nameInput = nullptr;
        }
        if (m_nameInput) {
            m_nameInput->render();
        }
    }
}

void UILayer::handleEvent(SDL_Event& event)
{
    if (m_nameInput) {
        m_nameInput->handleEvent(event);
    }
}

bool UILayer::isGameOver()
{
    PlayerShip* player = getPlayer();
    return !player->isAlive() && !player->isSpawning() && m_appState->properties["playerShips"].integer <= 0;
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
    auto* player = static_cast<PlayerShip*>(m_appState->scene->manager.findLayerByName("PlayerShip"));
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
