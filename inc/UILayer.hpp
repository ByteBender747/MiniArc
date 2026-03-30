#pragma once

#include "AppLayer.hpp"
#include "FontRenderer.hpp"
#include "ResPtr.hpp"
#include "TextInput.hpp"
#include <SDL3/SDL_render.h>

struct MiniArcGame;
struct GameAssets;
class PlayerShip;

class NameInput
{
public:
    NameInput(sdlc::AppState* appState, sdlc::FontRenderer* font);
    const std::string& getName() { return m_name; }
    bool hasFinished() const { return m_inputFinished; }
    void render();
    void handleEvent(SDL_Event& event);
private:
    bool m_inputFinished{false};
    std::string m_name;
    sdlc::FontRenderer *m_font;
    sdlc::AppState *m_appState;
    sdlc::TextInput m_textInput;
};

class UILayer : public sdlc::AppLayer
{
public:
    UILayer(MiniArcGame* game);
    ~UILayer();
    void render(SDL_Renderer* renderer) override;
    void update(float deltaTime) override;
    void handleEvent(SDL_Event& event) override;
private:
    PlayerShip* getPlayer();
    bool isGameOver();
    void renderScoreValue();
    void renderHealthBar();
    void renderShipCount();
    void renderModTime();
private:
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
