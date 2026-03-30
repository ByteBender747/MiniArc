#pragma once

#include <SDL3/SDL_render.h>
#include <cstdint>

#include "AppState.hpp"
#include "AudioDataBuffer.hpp"
#include "SpriteRenderer.hpp"
#include "ResPtr.hpp"

struct GameAssets
{
    sdlc::SpriteDefinitions sprites;
    sdlc::ResPtr<SDL_Texture> spriteTexture;
    sdlc::AudioDataBuffer laserShot;
    sdlc::AudioDataBuffer alienShot;
    sdlc::AudioDataBuffer explosion;
    sdlc::AudioDataBuffer spawnEffect;
    sdlc::AudioDataBuffer hitEffect;
    sdlc::AudioDataBuffer gameOver;
    sdlc::AudioDataBuffer pickup;
    sdlc::AudioDataBuffer charging;
    sdlc::AudioDataBuffer chargedShot;
};

struct HiScoreRecord
{
    std::string playerName;
    uint32_t score;
};

using HiScoreTable = std::array<HiScoreRecord, 10>;

class PlayerShip;
class BackgroundStars;
class EnemySpawner;
class UILayer;

struct MiniArcGame : sdlc::Scene
{
    MiniArcGame(sdlc::AppState *appState);
    GameAssets assets;
    PlayerShip* player{nullptr};
    BackgroundStars* stars{nullptr};
    EnemySpawner* enemies{nullptr};
    UILayer* uiLayer{nullptr};
};

uint32_t LoadHiScore(HiScoreTable &table);
void SaveHiScore(HiScoreTable &table);
