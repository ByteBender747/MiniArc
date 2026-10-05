#pragma once

#include "AppLayer.hpp"
#include "FontRenderer.hpp"
#include "MiniArc.hpp"
#include "Rect.hpp"
#include "TextInput.hpp"
#include "Archive.hpp"
#include <SDL3/SDL_render.h>

struct MiniArcGame;
struct GameAssets;
class PlayerShip;

class NameInput : public sdlc::AppLayer
{
public:
    NameInput(sdlc::AppState* appState, sdlc::FontRenderer* font);
    ~NameInput() override;
    void render(SDL_Renderer *renderer) override;
    void update(float deltaTime) override;
    void handleEvent(SDL_Event& event) override;
private:
    HiScoreTable m_table;
    sdlc::FontRenderer *m_font;
    sdlc::AppState *m_appState;
    sdlc::TextInput m_textInput;
};

class HiScoreScreen : public sdlc::AppLayer
{
public:
    HiScoreScreen(sdlc::FontRenderer* font);
    void render(SDL_Renderer *renderer) override;
    void update(float deltaTime) override;
    void printScoreLine(int line);
private:
    HiScoreTable m_scoreTable;
    sdlc::Point2D<float> m_posOffset;
    sdlc::FontRenderer *m_font;
};

class UILayer : public sdlc::AppLayer
{
public:
    UILayer(MiniArcGame* game, sdlc::Archive& archive);
    ~UILayer();
    void render(SDL_Renderer* renderer) override;
    void update(float deltaTime) override;
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
    bool m_nameInputStarted{false};
    GameAssets* m_assets;
    sdlc::FontRenderer* m_font;
    sdlc::AppState* m_appState;
};
