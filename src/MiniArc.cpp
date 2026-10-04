#include <SDL3/SDL_log.h>
#include <string>

#include <SDL3/SDL.h>

#include "AppState.hpp"
#include "BackgroundStars.hpp"
#include "Enemies.hpp"
#include "PlayerShip.hpp"
#include "MiniArc.hpp"
#include "UILayer.hpp"
#include "Utility.hpp"
#include "ResPtr.hpp"
#include "ConfigFile.hpp"
#include "Archive.hpp"

void AppLoadConfig(sdlc::AppConfig &config)
{
    sdlc::ConfigFile iniFile;
    std::filesystem::path cfgPath = sdlc::GetSaveGameFolder(ORG_NAME, APP_NAME) / "config.ini";
    std::string temp = cfgPath.string();
    if (iniFile.read(cfgPath)) {
        SDL_Log("Load config file: %s", temp.c_str());
        iniFile.beginSection("Window");
        iniFile.getValue<int>(config.windowWidth, "width");
        iniFile.getValue<int>(config.windowHeight, "height");
        if (iniFile.exists("pixelSize")) {
            int pixelSize;
            iniFile.getValue<int>(pixelSize, "pixelSize");
            config.logicalWidth = config.windowWidth / pixelSize;
            config.logicalHeight = config.windowHeight / pixelSize;
        } else {
            iniFile.getValue<int>(config.logicalWidth, "logicalWidth");
            iniFile.getValue<int>(config.logicalHeight, "logicalHeight");
        }
        iniFile.getValue<int>(config.vSync, "vSync");
        iniFile.endSection();
    } else {
        SDL_Log("Try to load configuration: %s", temp.c_str());
        SDL_Log("No application configuration found using defaults");
    }
}

MiniArcGame::MiniArcGame(sdlc::AppState *appState)
    : Scene(appState)
{
    sdlc::Archive arch(sdlc::ResolveRelativeToExe("MiniArc.dat").c_str());
    assets.spriteTexture = arch.loadTexture(appState->renderer, "Assets/arcade.png");
    assets.alienShot.loadWave(appState->audio.audioSpec, arch, "Assets/alien-shot.wav");
    assets.laserShot.loadWave(appState->audio.audioSpec, arch, "Assets/laser-shot.wav");
    assets.explosion.loadWave(appState->audio.audioSpec, arch, "Assets/explosion.wav");
    assets.spawnEffect.loadWave(appState->audio.audioSpec, arch, "Assets/spawn-effect.wav");
    assets.hitEffect.loadWave(appState->audio.audioSpec, arch, "Assets/hit-effect.wav");
    assets.gameOver.loadWave(appState->audio.audioSpec, arch, "Assets/game-over.wav");
    assets.pickup.loadWave(appState->audio.audioSpec, arch, "Assets/pickup.wav");
    assets.charging.loadWave(appState->audio.audioSpec, arch, "Assets/charging.wav");
    assets.chargedShot.loadWave(appState->audio.audioSpec, arch, "Assets/charged-shot..wav");
    auto buffer = arch.extract("Assets/arcade.map");
    sdlc::LoadSpriteDefinitions(assets.sprites, std::istream(&buffer));
    appState->input.keys.mapKey("shipUp", SDL_SCANCODE_W);
    appState->input.keys.mapKey("shipDown", SDL_SCANCODE_S);
    appState->input.keys.mapKey("shipLeft", SDL_SCANCODE_A);
    appState->input.keys.mapKey("shipRight", SDL_SCANCODE_D);
    appState->input.keys.mapKey("fireBeam", SDL_SCANCODE_SPACE);
    appState->properties["score"].integer = 0;
    appState->properties["playerShips"].integer = 2;
    appState->properties["gameOver"].boolean = false;

    stars = new BackgroundStars(this);
    player = new PlayerShip(this);
    enemies = new EnemySpawner(this);
    uiLayer = new UILayer(this);

    manager.addLayer(stars);
    manager.addLayer(player);
    manager.addLayer(enemies);
    manager.addLayer(uiLayer);
}
