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

// Audio stream assignment
constexpr int strmPlayerGun = 0;
constexpr int strmAlienGun = 1;
constexpr int strmExplosions = 2;
constexpr int strmPlayerEffects = 3;

// Drawing order configuration
constexpr int backgroundZIndex = 0;
constexpr int enemyZIndex = 1;
constexpr int playerZIndex = 2;
constexpr int uiZIndex = 3;