#include <cassert>
#include <format>
#include <string>

#include "UILayer.hpp"
#include "AppLayer.hpp"
#include "AppState.hpp"
#include "Dimension.hpp"
#include "FontRenderer.hpp"
#include "GameConfig.hpp"
#include "HealthBar.hpp"
#include "MiniArc.hpp"
#include "PlayerShip.hpp"
#include "Rect.hpp"
#include "SpriteRenderer.hpp"
#include "TextInput.hpp"
#include "Utility.hpp"

HiScoreScreen::HiScoreScreen(sdlc::FontRenderer *font)
    : sdlc::AppLayer("HiScoreScreen", uiZIndex)
      , m_font(font)
{
    LoadHiScore(m_scoreTable);
    m_posOffset = {RENDER_LOGICAL_WIDTH / 2 - 40, RENDER_LOGICAL_HEIGHT / 2};
}

void HiScoreScreen::printScoreLine(int line)
{
    float oldScale = m_font->scale();
    std::string outputStr = std::format("{}. {}", line + 1, m_scoreTable[line].playerName);
    sdlc::Dimension<float> dim = m_font->measureText(outputStr);
    m_font->setScale(.15f);
    m_font->setTextColor(sdlc::RGBA(255, 255, 255, 255));
    m_font->renderText(m_posOffset.x, m_posOffset.y + line * dim.height, outputStr);
    outputStr = std::format("{:7}", m_scoreTable[line].score);
    m_font->renderText(m_posOffset.x + 60, m_posOffset.y + line * dim.height, outputStr);
    m_font->setScale(oldScale);
}

void HiScoreScreen::render(SDL_Renderer *renderer)
{
    for (int i = 0; i < 10; i++) {
        printScoreLine(i);
    }
}

void HiScoreScreen::update(float deltaTime)
{
}

NameInput::NameInput(sdlc::AppState *appState, sdlc::FontRenderer *font)
    : sdlc::AppLayer("UIInput", uiZIndex)
      , m_textInput(appState->renderer, font)
      , m_appState(appState)
      , m_font(font)
{
    uint32_t currentScore = m_appState->properties["score"].integer;
    uint32_t minScore = LoadHiScore(m_table);
    if (currentScore > minScore) {
        sdlc::TextInput::startTextInput(m_appState->window);
        m_textInput.setCallback([this, currentScore](std::string_view text, bool accepted) {
            if (accepted) {
                int rating = ScoreRating(m_table, currentScore);
                if (rating > -1) {
                    m_table[rating].playerName = text;
                    m_table[rating].score = currentScore;
                    SaveHiScore(m_table);
                }
            }
            deleteLayer();
            m_appState->scene->manager.addLayer(new HiScoreScreen(m_font));
        });
    } else {
        deleteLayer();
        m_appState->scene->manager.addLayer(new HiScoreScreen(font));
    }
}

NameInput::~NameInput()
{
    sdlc::TextInput::stopTextInput(m_appState->window);
}

void NameInput::render(SDL_Renderer *renderer)
{
    float oldScale = m_font->scale();
    std::string caption = "Your Name: ";
    sdlc::Point2D<float> pos(RENDER_LOGICAL_WIDTH / 2 - 20, RENDER_LOGICAL_HEIGHT / 2);
    m_font->setScale(0.15);
    m_font->setTextColor(sdlc::RGBA(255, 255, 255, 255));
    m_font->renderText(pos.x, pos.y, caption);
    m_textInput.render(pos.x + m_font->measureText(caption).width, pos.y);
    m_font->setScale(oldScale);
}

void NameInput::update(float deltaTime)
{
}

void NameInput::handleEvent(SDL_Event &event)
{
    m_textInput.handleEvent(event);
}

UILayer::UILayer(MiniArcGame *game)
    : AppLayer("UILayer", uiZIndex)
      , m_appState(game->appState)
      , m_assets(&game->assets)
{
    m_font = new sdlc::FontRenderer(
        m_appState->renderer,
        sdlc::ResolveRelativeToExe("../Assets/Gameplay.ttf").string().c_str(),
        32, sdlc::FontRenderMode::Solid);
    m_startImage = sdlc::LoadTexture(m_appState->renderer, sdlc::ResolveRelativeToExe("../Assets/start.png").c_str());
    m_gameOverImage = sdlc::LoadTexture(m_appState->renderer,
                                        sdlc::ResolveRelativeToExe("../Assets/game_over.png").c_str());
    m_font->setScale(0.25);
}

UILayer::~UILayer()
{
    delete m_font;
}

void UILayer::update(float deltaTime)
{
}

void UILayer::render(SDL_Renderer *renderer)
{
    renderScoreValue();
    renderHealthBar();
    renderShipCount();
    renderModTime();
    if (isGameOver()) {
        sdlc::SpriteRenderer sr(m_gameOverImage);
        sr.setPosition(RENDER_LOGICAL_WIDTH / 2., RENDER_LOGICAL_HEIGHT / 2. - 10);
        sr.render(renderer);
        if (!m_gameOverSfxFlag) {
            m_appState->audio.stream[strmPlayerEffects].setGain(.35f);
            m_appState->audio.stream[strmPlayerEffects].put(m_assets->gameOver);
            m_gameOverSfxFlag = true;
        }
        m_appState->properties["gameOver"].boolean = true;
        if (!m_nameInputStarted) {
            m_appState->scene->manager.addLayer(new NameInput(m_appState, m_font));
            m_nameInputStarted = true;
        }
    }
}

bool UILayer::isGameOver()
{
    PlayerShip *player = getPlayer();
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

PlayerShip *UILayer::getPlayer()
{
    auto *player = static_cast<PlayerShip *>(m_appState->scene->manager.findLayerByName("PlayerShip"));
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
