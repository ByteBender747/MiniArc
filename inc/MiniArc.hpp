#pragma once

#include <memory>

#include "SpriteRenderer.hpp"
#include <SDL3/SDL_render.h>

class PlayerShip;

struct GameState
{
    std::unique_ptr<PlayerShip> player;
    sdl::SpriteDefinitions sprites;
    SDL_Texture* spriteTexture;
};