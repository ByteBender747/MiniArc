#pragma once

#include <memory>
#include <SDL3/SDL_render.h>

#include "AudioDataBuffer.hpp"
#include "SpriteRenderer.hpp"
#include "ResPtr.hpp"

struct GameAssets
{
    sdlc::SpriteDefinitions sprites;
    sdlc::ResPtr<SDL_Texture> spriteTexture;
    std::unique_ptr<sdlc::AudioDataBuffer> laserShot;
    std::unique_ptr<sdlc::AudioDataBuffer> alienShot;
};

// Audio stream assignment
constexpr int strmPlayerGun = 0;
constexpr int strmAlienGun = 1;

// Drawing order configuration
constexpr int backgroundZIndex = 0;
constexpr int enemyZIndex = 1;
constexpr int playerZIndex = 2;
constexpr int uiZIndex = 3;