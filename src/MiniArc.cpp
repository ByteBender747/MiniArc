#include <ostream>
#include <iostream>

#include <SDL3/SDL_scancode.h>

#include "AppState.hpp"
#include "BackgroundStars.hpp"
#include "Enemies.hpp"
#include "PlayerShip.hpp"
#include "MiniArc.hpp"
#include "Utility.hpp"
#include "UILayer.hpp"

#include <SDL3_image/SDL_image.h>

void MiniArc_Init(sdlc::AppState *state, int argc, char **argv)
{
    auto assets = new GameAssets();
    if (!sdlc::loadSpriteDefinitions(assets->sprites, sdlc::resolveRelativeToExe("../Assets/arcade.map"))) {
        std::cerr << "Error: Could not load Assets/arcade.txt" << std::endl;
        goto ErrorExit;
    }
    assets->spriteTexture = IMG_LoadTexture(state->renderer,
                                            sdlc::resolveRelativeToExe("../Assets/arcade.png").c_str());
    if (!assets->spriteTexture) {
        std::cerr << "Error: Could not load Assets/arcade.png" << std::endl;
        goto ErrorExit;
    }
    sdlc::loadWaveRelative(assets->alienShot, state, "../Assets/alien-shot.wav");
    sdlc::loadWaveRelative(assets->laserShot, state, "../Assets/laser-shot.wav");
    sdlc::loadWaveRelative(assets->explosion, state, "../Assets/explosion.wav");
    sdlc::loadWaveRelative(assets->spawnEffect, state, "../Assets/spawn-effect.wav");
    sdlc::loadWaveRelative(assets->hitEffect, state, "../Assets/hit-effect.wav");
    sdlc::loadWaveRelative(assets->gameOver, state, "../Assets/game-over.wav");
    sdlc::loadWaveRelative(assets->pickup, state, "../Assets/pickup.wav");
    sdlc::loadWaveRelative(assets->charging, state, "../Assets/charging.wav");
    sdlc::loadWaveRelative(assets->chargedShot, state, "../Assets/charged-shot.wav");
    state->input.keys.mapKey("shipUp", SDL_SCANCODE_W);
    state->input.keys.mapKey("shipDown", SDL_SCANCODE_S);
    state->input.keys.mapKey("shipLeft", SDL_SCANCODE_A);
    state->input.keys.mapKey("shipRight", SDL_SCANCODE_D);
    state->input.keys.mapKey("fireBeam", SDL_SCANCODE_SPACE);
    state->properties["assets"].pointer = assets;
    state->properties["player"].pointer = new PlayerShip(state, assets->spriteTexture);
    state->properties["stars"].pointer = new BackgroundStars(state, assets->spriteTexture);
    state->properties["enemies"].pointer = new EnemySpawner(state);
    state->properties["ui"].pointer = new UILayer(state);
    state->properties["score"].integer = 0;
    state->properties["playerShips"].integer = 2;
    state->properties["gameOver"].boolean = false;
    return;
ErrorExit:
    state->isRunning = false;
    delete assets;
}

void MiniArc_Exit(sdlc::AppState *state)
{
    GameAssets *assets = static_cast<GameAssets *>(state->properties["assets"].pointer);
    deleteEnemies(state, "enemies");
    deleteBackgroundStar(state, "stars");
    deletePlayerShip(state, "player");
    deleteUI(state, "ui");
    delete assets;
}
