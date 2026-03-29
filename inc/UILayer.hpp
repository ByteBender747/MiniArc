#pragma once

#include "FontRenderer.hpp"
#include "MiniArc.hpp"
#include "PlayerShip.hpp"
#include "ResPtr.hpp"
#include <SDL3/SDL_render.h>

#include "TextInput.hpp"

class NameInput
{
public:
    NameInput(sdlc::AppState* appState, sdlc::FontRenderer* font);
    ~NameInput();
    const std::string& getName() { return m_name; }
    bool hasFinished() const { return m_inputFinished; }
private:
    bool m_inputFinished{false};
    std::string m_name;
    sdlc::FontRenderer *m_font;
    sdlc::AppState *m_appState;
    sdlc::TextInput m_textInput;
};

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
    bool m_hiscoreDone{false};
    GameAssets* m_assets;
    NameInput* m_nameInput{nullptr};
    sdlc::ResPtr<SDL_Texture> m_startImage;
    sdlc::ResPtr<SDL_Texture> m_gameOverImage;
    sdlc::FontRenderer* m_font;
    sdlc::AppState* m_appState;
};

void deleteUI(sdlc::AppState* state, const std::string& name);