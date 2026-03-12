#pragma once

#include "SpriteRenderer.hpp"
#include <SDL3/SDL_render.h>

struct GameState
{
    sdl::SpriteDefinitions sprites;
    SDL_Texture* spriteTexture;
};