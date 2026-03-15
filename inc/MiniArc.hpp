#pragma once

#include "SpriteRenderer.hpp"
#include <SDL3/SDL_render.h>
#include "ResPtr.hpp"

struct GameAssets
{
    sdlc::SpriteDefinitions sprites;
    sdlc::ResPtr<SDL_Texture> spriteTexture;
};

// Drawing order configuration
constexpr int backgroundZIndex = 0;
constexpr int enemyZIndex = 1;
constexpr int playerZIndex = 2;
constexpr int uiZIndex = 3;