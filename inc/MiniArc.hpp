#pragma once

#include <memory>

#include "SpriteRenderer.hpp"
#include <SDL3/SDL_render.h>

struct GameAssets
{
    sdl::SpriteDefinitions sprites;
    SDL_Texture* spriteTexture;
};