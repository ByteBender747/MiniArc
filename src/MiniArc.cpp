#include <SDL3/SDL_surface.h>
#include <iostream>

#include "AppEntry.hpp"
#include "PlayerShip.hpp"
#include "SpriteRenderer.hpp"
#include "MiniArc.hpp"

#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>
#include <memory>
#include <ostream>

void MiniArc_Init(AppState* state, int argc, char** argv)
{
    GameState* gs = new GameState();
    state->userdata = gs;
    if (!sdl::LoadSpriteDefinitions(gs->sprites, "../Assets/arcade.txt")) {
        std::cerr << "Error: Could not load Assets/arcade.txt" << std::endl;
        state->isRunning = false;
        return;
    }
    gs->spriteTexture = IMG_LoadTexture(state->renderer, "../Assets/arcade.png");
    if (!gs->spriteTexture) {
        std::cerr << "Error: Could not load Assets/arcade.png" << std::endl;
        state->isRunning = false;
        return;
    }
    gs->player = std::make_unique<PlayerShip>(state, gs->spriteTexture);
}

void MiniArc_Exit(AppState* state)
{
    GameState* gs = static_cast<GameState*>(state->userdata);
    SDL_DestroyTexture(gs->spriteTexture);
    delete gs;
}