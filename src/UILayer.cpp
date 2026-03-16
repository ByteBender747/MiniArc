#include <ios>
#include <sstream>

#include "UILayer.hpp"
#include "AppState.hpp"
#include "FontRenderer.hpp"
#include "MiniArc.hpp"
#include "PathHelper.hpp"
#include "PlayerShip.hpp"
#include "SpriteRenderer.hpp"

#include <SDL3_image/SDL_image.h>

UILayer::UILayer(sdlc::AppState* appState)
    : m_appState(appState)
{
    m_font = new sdlc::FontRenderer(
        m_appState->renderer,
        sdlc::resolveRelativeToExe("../Assets/Gameplay.ttf").c_str(), 
        32, sdlc::FontRenderMode::Solid);
    m_appState->iterateHandler[uiZIndex] = [this](sdlc::AppState* appState) {
        renderScoreValue();
        if (isGameOver()) {
            sdlc::SpriteRenderer sr(m_gameOverImage);
            sr.setPosition(RENDER_LOGICAL_WIDTH / 2., RENDER_LOGICAL_HEIGHT / 2.);
            sr.render(m_appState->renderer);
        }
    };
    m_startImage = IMG_LoadTexture(m_appState->renderer, sdlc::resolveRelativeToExe("../Assets/start.png").c_str());
    m_gameOverImage = IMG_LoadTexture(m_appState->renderer, sdlc::resolveRelativeToExe("../Assets/game_over.png").c_str());
}

bool UILayer::isGameOver()
{
    PlayerShip* player = static_cast<PlayerShip*>(m_appState->properties["player"].pointer);
    return !player->isAlive() && m_appState->properties["playerShips"].integer <= 0;
}

UILayer::~UILayer()
{
    m_appState->iterateHandler[uiZIndex] = nullptr;
    delete m_font;
}

void UILayer::renderScoreValue()
{
    int64_t score = m_appState->properties["score"].integer;
    std::ostringstream os;
    os.setf(std::ios_base::showbase);
    os << std::setw(7) << std::setfill('0') << score;
    auto dim = m_font->measureText(os.str().c_str());
    m_font->renderText(WINDOW_WIDTH - dim.width - 20, 10, os.str().c_str());
}

void deleteUI(sdlc::AppState* state, const std::string& name)
{
    UILayer* ptr = static_cast<UILayer*>(state->properties[name].pointer);
    if (ptr) {
        delete ptr;
        state->properties[name].pointer = nullptr;
    }
}