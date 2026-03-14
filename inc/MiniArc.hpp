#pragma once

#include "SpriteRenderer.hpp"
#include <SDL3/SDL_render.h>
#include "ResPtr.hpp"

struct GameAssets
{
    sdlc::SpriteDefinitions sprites;
    sdlc::ResPtr<SDL_Texture> spriteTexture;
};