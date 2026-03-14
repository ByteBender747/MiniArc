#pragma once

#include "SpriteRenderer.hpp"
#include <SDL3/SDL_render.h>
#include "ResPtr.hpp"

struct GameAssets
{
    sdl::SpriteDefinitions sprites;
    sdl::ResPtr<SDL_Texture> spriteTexture;
};