#include <ios>
#include <sstream>

#include "UILayer.hpp"
#include "AppState.hpp"
#include "FontRenderer.hpp"
#include "MiniArc.hpp"
#include "PathHelper.hpp"

UILayer::UILayer(sdlc::AppState* appState)
    : m_appState(appState)
{
    m_font = new sdlc::FontRenderer(
        m_appState->renderer,
        sdlc::resolveRelativeToExe("../Assets/Gameplay.ttf").c_str(), 
        32, sdlc::FontRenderMode::Solid);
    m_appState->iterateHandler[uiZIndex] = [this](sdlc::AppState* appState) {
        renderScoreValue();
    };
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