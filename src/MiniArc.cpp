#include <memory>
#include <ostream>
#include <iostream>

#include <SDL3/SDL_surface.h>

#include "AppState.hpp"
#include "PlayerShip.hpp"
#include "SpriteRenderer.hpp"
#include "MiniArc.hpp"
#include "PathHelper.hpp"

#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>

void MiniArc_Init(sdl::AppState* state, int argc, char** argv)
{
    GameAssets* assets = new GameAssets();
    if (!sdl::loadSpriteDefinitions(assets->sprites, sdl::resolveRelativeToExe("../Assets/arcade.txt"))) {
        std::cerr << "Error: Could not load Assets/arcade.txt" << std::endl;
        state->isRunning = false;
        return;
    }
    assets->spriteTexture = IMG_LoadTexture(state->renderer, sdl::resolveRelativeToExe("../Assets/arcade.png").c_str());
    if (!assets->spriteTexture) {
        std::cerr << "Error: Could not load Assets/arcade.png" << std::endl;
        state->isRunning = false;
        return;
    }
    state->properties["assets"].pointer = assets;
    state->properties["player"].pointer = new PlayerShip(state, assets->spriteTexture);
}

void MiniArc_Exit(sdl::AppState* state)
{
    GameAssets* assets = static_cast<GameAssets*>(state->properties["assets"].pointer);
    PlayerShip* player = static_cast<PlayerShip*>(state->properties["player"].pointer);
    delete player;
    SDL_DestroyTexture(assets->spriteTexture);
    delete assets;
}