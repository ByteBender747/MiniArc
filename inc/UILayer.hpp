#pragma once

#include "AppState.hpp"
#include "FontRenderer.hpp"

class UILayer
{
public:
    UILayer(sdlc::AppState* appState);
    virtual ~UILayer();
    void renderScoreValue();
private:
    sdlc::FontRenderer* m_font;
    sdlc::AppState* m_appState;
};

void deleteUI(sdlc::AppState* state, const std::string& name);