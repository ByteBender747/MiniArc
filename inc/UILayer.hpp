#pragma once

#include "AppState.hpp"
#include "FontRenderer.hpp"
#include "ResPtr.hpp"
#include <SDL3/SDL_render.h>

class UILayer
{
public:
    UILayer(sdlc::AppState* appState);
    virtual ~UILayer();
    void renderScoreValue();
    bool isGameOver();
private:
    sdlc::ResPtr<SDL_Texture> m_startImage;
    sdlc::ResPtr<SDL_Texture> m_gameOverImage;
    sdlc::FontRenderer* m_font;
    sdlc::AppState* m_appState;
};

void deleteUI(sdlc::AppState* state, const std::string& name);