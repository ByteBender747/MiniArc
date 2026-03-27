#pragma once

#include "FontRenderer.hpp"
#include "MiniArc.hpp"
#include "PlayerShip.hpp"
#include "ResPtr.hpp"
#include <SDL3/SDL_render.h>

class UILayer
{
public:
    UILayer(sdlc::AppState* appState);
    virtual ~UILayer();
    void renderScoreValue();
    void renderHealthBar();
    void renderShipCount();
    void renderModTime();
    bool isGameOver();
private:
    PlayerShip* getPlayer();
    int m_renderWidth{};
    int m_renderHeight{};
    bool m_gameOverSfxFlag{false};
    GameAssets* m_assets;
    sdlc::ResPtr<SDL_Texture> m_startImage;
    sdlc::ResPtr<SDL_Texture> m_gameOverImage;
    sdlc::FontRenderer* m_font;
    sdlc::AppState* m_appState;
};

void deleteUI(sdlc::AppState* state, const std::string& name);