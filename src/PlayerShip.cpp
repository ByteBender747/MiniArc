#include <cassert>

#include "PlayerShip.hpp"
#include "AnimatedSprite.hpp"
#include "AppState.hpp"
#include "Enemies.hpp"
#include "MathFunc.hpp"
#include "MiniArc.hpp"
#include "Rect.hpp"
#include "SpriteRenderer.hpp"
#include "Vector2.hpp"
#include "Keyboard.hpp"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_scancode.h>

using namespace sdlc;

constexpr float playerMoveSpeed = 100;
constexpr int playerBeamDamage = 50;

PlayerShip::PlayerShip(AppState* state, SDL_Texture* texture)
    : m_appState(state)
    , m_sprite(texture)
    , m_flames(texture)
    , m_shotSprite(texture)
    , m_deathAnimation(texture)
    , m_spawnEffect(texture)
{
    m_assets = static_cast<GameAssets*>(state->properties["assets"].pointer);
    m_sprite.setScaleMode(SDL_SCALEMODE_NEAREST);
    m_sprite.setSource(m_assets->sprites["PlayerCenter"]);
    m_flames.setSource(m_assets->sprites["FlameLeft1"]);
    m_position = sdlc::Vec2f(RENDER_LOGICAL_WIDTH / 2., RENDER_LOGICAL_HEIGHT - 20.);
    m_posLimits[0] = 7;
    m_posLimits[1] = RENDER_LOGICAL_WIDTH - 7;
    m_posLimits[2] = 8;
    m_posLimits[3] = RENDER_LOGICAL_HEIGHT - 8;
    m_shotSprite.setSource(m_assets->sprites["Laser"]);
    m_appState->iterateHandler[playerZIndex] = [this](AppState* appState) {
        if (!m_spawnEffect.isFinished()) {
            m_spawnEffect.setPosition(m_position.x, m_position.y);
            m_spawnEffect.update(appState->deltaTime);
            m_spawnEffect.render(appState->renderer);
        } else {
            iteratePlayerShip();
        }
    };
    m_appState->eventHandler[playerZIndex] = [this](AppState* appState, SDL_Event* event) {
        if (event->type == SDL_EVENT_KEY_UP && m_triggerState) {
            if (!sdlc::getKeyState(SDL_SCANCODE_SPACE)) {
                m_triggerState = false;
            }
        }
    };

    m_deathAnimation.addFrame(m_assets->sprites["Boom1"]);
    m_deathAnimation.addFrame(m_assets->sprites["Boom2"]);
    m_deathAnimation.addFrame(m_assets->sprites["Boom3"]);
    m_deathAnimation.addFrame(m_assets->sprites["Boom4"]);
    m_deathAnimation.addFrame(m_assets->sprites["Boom5"]);
    m_deathAnimation.setFPS(20);
    m_deathAnimation.setRepeat(false);
    m_deathAnimation.animationFinished([this](sdlc::AnimatedSprite& sprite) {
        reSpawn();
    });

    sdlc::SpriteImageDistribution dist {
        .startX = 3,
        .startY = 71,
        .spriteWidth = 14,
        .spriteHeight = 14,
        .marginX = 2,
        .marginY = 0,
        .spritesPerLine = 8,
        .spriteCount = 8
    };
    m_spawnEffect.addFrames(dist);
    m_spawnEffect.setDuration(1.5);
    m_spawnEffect.setRepeat(false);
    m_spawnEffect.play();
    m_spawnEffect.animationFinished([this](sdlc::AnimatedSprite& sprite) {
        m_isAlive = true;
    });
}

PlayerShip::~PlayerShip()
{
    m_appState->iterateHandler[playerZIndex] = nullptr;
    m_appState->eventHandler[playerZIndex] = nullptr;
}

void PlayerShip::iteratePlayerShip()
{
    if (m_isAlive) {
        handleInputs();
        shipMovement();
        animateFlames();
    }
    moveAndRenderProjectiles(200);
    if (m_hitPoints > 0) {
        if (m_hitFlash) {
            SDL_SetRenderColorScale(m_appState->renderer, 255);
            --m_hitFlash;
        }
        m_sprite.render(m_appState->renderer);
        SDL_SetRenderColorScale(m_appState->renderer, 1);
        m_flames.render(m_appState->renderer);
    } else {
        if (m_isAlive) {
            m_deathAnimation.play();
            m_isAlive = false;
        }
        m_deathAnimation.setPosition(m_position.x, m_position.y);
        m_deathAnimation.update(m_appState->deltaTime);
        m_deathAnimation.render(m_appState->renderer);
    }
}

bool PlayerShip::fireProjectile(float x, float y)
{
    Projectile* projectile = m_projectiles.acquire();
    if (projectile)  {
        projectile->position = sdlc::Vec2f(x, y);
    }
    return projectile != nullptr;
}

void PlayerShip::reSpawn()
{
    if (m_appState->properties["playerShips"].integer > 0) {
        m_position = sdlc::Vec2f(RENDER_LOGICAL_WIDTH / 2., RENDER_LOGICAL_HEIGHT - 20.);
        m_hitPoints = playerInitialHitPoints;
        --m_appState->properties["playerShips"].integer;
        m_projectiles.clear();
        m_spawnEffect.play();
        m_appState->input.keys.clearStates();
        m_hitFlash = 0;
    }
}

bool PlayerShip::hitCheck(const SDL_FRect& rect, int damage)
{
    bool result = false;
    if (!isAlive() || isSpawning()) {
        return result;
    }
    sdlc::Rect<float> shipRect = m_sprite.destination();
    if (shipRect.intersects(rect)) {
        result = true;
        m_hitPoints -= damage;
        m_hitFlash = 5;
    }
    return result;
}

void PlayerShip::moveAndRenderProjectiles(float shotSpeed)
{
    EnemySpawner* enemies = static_cast<EnemySpawner*>(m_appState->properties["enemies"].pointer);
    assert(enemies);
    for (auto& item : m_projectiles) {
        if (item.acquired) {
            Projectile& projectile = item.value;
            projectile.position += sdlc::Vec2f(0, -shotSpeed * m_appState->deltaTime);
            m_shotSprite.setPosition(projectile.position.x, projectile.position.y, sdlc::SpritePositionOffset::Center);
            m_shotSprite.render(m_appState->renderer);
            if (projectile.position.y < 0) {
                item.acquired = false;
            }
            if (enemies->hitCheckAllEnemies(m_shotSprite.destination(), playerBeamDamage)) {
                item.acquired = false;
            }
        }
    }
}

void PlayerShip::animateFlames()
{
    m_flameTimer += m_appState->deltaTime;
    if (m_flameTimer > 0.05) {
        m_flameType = (m_flameType + 1) & 0x01;
        m_flameTimer = 0;
    }
    switch (m_direction) {
    case MovementDirection::Left:
        m_flames.setSource(m_flameType ? m_assets->sprites["FlameLeft1"] : m_assets->sprites["FlameLeft2"]);
        break;
    case MovementDirection::Right:
        m_flames.setSource(m_flameType ? m_assets->sprites["FlameRight1"] : m_assets->sprites["FlameRight2"]);
        break;
    case MovementDirection::None:
        m_flames.setSource(m_flameType ? m_assets->sprites["FlameCenter1"] : m_assets->sprites["FlameCenter2"]);
        break;
    }
}

void PlayerShip::handleInputs()
{
    float delta = m_appState->deltaTime * playerMoveSpeed;
    m_direction = MovementDirection::None;
    if (m_appState->input.keys.down("shipRight")) {
        m_position += sdlc::Vec2f(delta, 0);
        m_direction = MovementDirection::Right;
    }
    if (m_appState->input.keys.down("shipLeft")) {
        m_position += sdlc::Vec2f(-delta, 0);
        m_direction = MovementDirection::Left;
    }
    if (m_appState->input.keys.down("shipUp")) {
        m_position += sdlc::Vec2f(0, -delta);
    }
    if (m_appState->input.keys.down("shipDown")) {
        m_position += sdlc::Vec2f(0, delta);
    }
    if (m_appState->input.keys.fallingEdge("fireBeam")) {
        fireProjectile(m_position.x - 4, m_position.y);
        fireProjectile(m_position.x + 4, m_position.y);
        m_appState->audio.stream[strmPlayerGun]->clear();
        m_appState->audio.stream[strmPlayerGun]->put(*m_assets->laserShot);
    }
}

void PlayerShip::shipMovement()
{
    m_position.x = sdlc::clamp(m_position.x, m_posLimits[0], m_posLimits[1]);
    m_position.y = sdlc::clamp(m_position.y, m_posLimits[2], m_posLimits[3]);
    m_sprite.setPosition(m_position.x, m_position.y, sdlc::SpritePositionOffset::Center);
    switch (m_direction) {
    case MovementDirection::Left:
        m_sprite.setSource(m_assets->sprites["PlayerLeft"]);
        m_flames.setPosition(m_position.x - 1, m_position.y + 10, sdlc::SpritePositionOffset::Center);
        break;
    case MovementDirection::Right:
        m_sprite.setSource(m_assets->sprites["PlayerRight"]);
        m_flames.setPosition(m_position.x + 1, m_position.y + 10, sdlc::SpritePositionOffset::Center);
        break;
    case MovementDirection::None:
        m_sprite.setSource(m_assets->sprites["PlayerCenter"]);
        m_flames.setPosition(m_position.x, m_position.y + 10, sdlc::SpritePositionOffset::Center);
        break;
    }
}

void deletePlayerShip(AppState* state, const std::string& name)
{
    auto* player = static_cast<PlayerShip*>(state->properties[name].pointer);
    if (player) {
        delete player;
        state->properties[name].pointer = nullptr;
    }
}