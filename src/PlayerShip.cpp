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

using namespace sdlc;

PlayerShip::PlayerShip(MiniArcGame *game)
    : AppLayer("PlayerShip", playerZIndex)
    , m_appState(game->appState)
    , m_assets(&game->assets)
    , m_sprite(game->assets.spriteTexture)
    , m_flames(game->assets.spriteTexture)
    , m_shotSprite(game->assets.spriteTexture)
    , m_deathAnimation(game->assets.spriteTexture)
    , m_spawnEffect(game->assets.spriteTexture)
    , m_chargedShot(game->assets.spriteTexture)
{
    m_chargedShot.addFrame(m_assets->sprites["LaserCharged1"]);
    m_chargedShot.addFrame(m_assets->sprites["LaserCharged2"]);
    m_chargedShot.setFPS(10);
    m_chargedShot.play();
    m_sprite.setScaleMode(SDL_SCALEMODE_NEAREST);
    m_sprite.setSource(m_assets->sprites["PlayerCenter"]);
    m_flames.setSource(m_assets->sprites["FlameLeft1"]);
    m_position = Vec2f(RENDER_LOGICAL_WIDTH / 2., RENDER_LOGICAL_HEIGHT - 20.);
    m_posLimits[0] = 7;
    m_posLimits[1] = RENDER_LOGICAL_WIDTH - 7;
    m_posLimits[2] = 8;
    m_posLimits[3] = RENDER_LOGICAL_HEIGHT - 8;
    m_shotSprite.setSource(m_assets->sprites["Laser"]);

    m_deathAnimation.addFrame(m_assets->sprites["Boom1"]);
    m_deathAnimation.addFrame(m_assets->sprites["Boom2"]);
    m_deathAnimation.addFrame(m_assets->sprites["Boom3"]);
    m_deathAnimation.addFrame(m_assets->sprites["Boom4"]);
    m_deathAnimation.addFrame(m_assets->sprites["Boom5"]);
    m_deathAnimation.setFPS(20);
    m_deathAnimation.setRepeat(false);
    m_deathAnimation.setCallback([this](AnimatedSprite& sprite, AnimationEvent event) {
        if (event == AnimationEvent::finished) reSpawn();
    });

    SpriteImageDistribution dist {
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
    m_spawnEffect.setCallback([this](AnimatedSprite& sprite, AnimationEvent event) {
        if (event == AnimationEvent::finished) m_isAlive = true;
        if (event == AnimationEvent::start) {
            m_appState->audio.stream[strmPlayerEffects].put(m_assets->spawnEffect);
        }
    });
    m_spawnEffect.play();
}

void PlayerShip::render(SDL_Renderer *renderer)
{
    if (!m_spawnEffect.finished()) {
        m_spawnEffect.setPosition(m_position.x, m_position.y);
        m_spawnEffect.render(renderer, m_appState->deltaTime);
        return;
    }
    if (m_hitPoints > 0) {
        if (m_hitFlash) {
            SDL_SetRenderColorScale(renderer, 255);
            --m_hitFlash;
        }
        m_sprite.render(renderer);
        SDL_SetRenderColorScale(renderer, 1);
        m_flames.render(renderer);
    } else {
        if (m_isAlive) {
            m_deathAnimation.play();
            m_isAlive = false;
            m_appState->audio.stream[strmExplosions].put(m_assets->explosion, true);
        }
        m_deathAnimation.setPosition(m_position.x, m_position.y);
        m_deathAnimation.render(renderer, m_appState->deltaTime);
    }
}

void PlayerShip::update(float deltaTime)
{
    if (m_isAlive) {
        handleInputs();
        shipMovement();
        animateFlames();
    }
    moveAndRenderProjectiles(200);
}

bool PlayerShip::fireProjectile(float x, float y, bool charged)
{
    Projectile* projectile = m_projectiles.acquire();
    if (projectile)  {
        projectile->position = Vec2f(x, y);
        projectile->charged = charged;
    }
    return projectile != nullptr;
}

void PlayerShip::reSpawn()
{
    if (m_appState->properties["playerShips"].integer > 0) {
        m_position = Vec2f(RENDER_LOGICAL_WIDTH / 2., RENDER_LOGICAL_HEIGHT - 20.);
        m_hitPoints = playerInitialHitPoints;
        --m_appState->properties["playerShips"].integer;
        m_projectiles.clear();
        m_spawnEffect.play();
        m_appState->input.keys.clearStates();
        m_hitFlash = 0;
        m_weaponPowerUpTimer = 0;
        m_invulnerableTimer = playerInvulnerableTime;
    }
}

bool PlayerShip::hitCheck(const SDL_FRect& rect, int damage)
{
    bool result = false;
    if (!isAlive() || isSpawning()) {
        return result;
    }
    if (m_invulnerableTimer > 0) {
        m_invulnerableTimer -= m_appState->deltaTime;
        return result;
    }
    Rect<float> shipRect = m_sprite.destination();
    if (shipRect.intersects(rect)) {
        result = true;
        m_hitPoints -= damage;
        if (damage > 0) {
            m_hitFlash = 5;
            m_appState->audio.stream[strmPlayerEffects].put(m_assets->hitEffect, true);
        }
        if (damage < 0) { // negative damaged used for healing by pickup
            if (m_hitPoints > playerInitialHitPoints) {
                m_hitPoints = playerInitialHitPoints;
            }
        }
    }
    return result;
}

void PlayerShip::enableWeaponPowerUp(float time)
{
    m_weaponPowerUpTimer += time;
}

void PlayerShip::moveAndRenderProjectiles(float shotSpeed)
{
    EnemySpawner* enemies = static_cast<EnemySpawner*>(m_appState->scene->manager.findLayerByName("EnemySpawner"));
    assert(enemies);
    for (auto& item : m_projectiles) {
        if (item.acquired) {
            Projectile& projectile = item.value;
            projectile.position += Vec2f(0, -shotSpeed * m_appState->deltaTime);
            Rect<float> destination;
            if (projectile.charged) {
                m_chargedShot.setPosition(projectile.position.x, projectile.position.y, SpritePositionOffset::Center);
                m_chargedShot.render(m_appState->renderer, m_appState->deltaTime);
                destination = m_chargedShot.destination();
            } else {
                m_shotSprite.setPosition(projectile.position.x, projectile.position.y, SpritePositionOffset::Center);
                m_shotSprite.render(m_appState->renderer);
                destination = m_shotSprite.destination();
            }
            if (projectile.position.y < 0) {
                item.acquired = false;
            }
            if (enemies->hitCheckAllEnemies(destination, projectile.charged ? playerChargedBeamDamage : playerBeamDamage)) {
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
        m_position += Vec2f(delta, 0);
        m_direction = MovementDirection::Right;
    }
    if (m_appState->input.keys.down("shipLeft")) {
        m_position += Vec2f(-delta, 0);
        m_direction = MovementDirection::Left;
    }
    if (m_appState->input.keys.down("shipUp")) {
        m_position += Vec2f(0, -delta);
    }
    if (m_appState->input.keys.down("shipDown")) {
        m_position += Vec2f(0, delta);
    }
    if (m_appState->input.keys.down("fireBeam")) {
        m_chargingTimer += m_appState->deltaTime;
        if (m_chargingTimer > 0.2 && !m_chargedFlag) {
            m_appState->audio.stream[strmPlayerGun].put(m_assets->charging);
            m_chargedFlag = true;
        }
    }
    if (m_weaponPowerUpTimer > 0) {
        m_weaponPowerUpTimer -= m_appState->deltaTime;
    }
    if (m_appState->input.keys.fallingEdge("fireBeam")) {
        bool charged = m_chargingTimer > 0.5 || m_weaponPowerUpTimer > 0;
        fireProjectile(m_position.x - 4, m_position.y, charged);
        fireProjectile(m_position.x + 4, m_position.y, charged);
        m_appState->audio.stream[strmPlayerGun].clear();
        if (charged) {
            m_appState->audio.stream[strmPlayerGun].put(m_assets->chargedShot);
        } else {
            m_appState->audio.stream[strmPlayerGun].put(m_assets->laserShot);
        }
        m_chargingTimer = 0;
        m_chargedFlag = false;
    }
}

void PlayerShip::shipMovement()
{
    m_position.x = clamp(m_position.x, m_posLimits[0], m_posLimits[1]);
    m_position.y = clamp(m_position.y, m_posLimits[2], m_posLimits[3]);
    m_sprite.setPosition(m_position.x, m_position.y, SpritePositionOffset::Center);
    switch (m_direction) {
    case MovementDirection::Left:
        m_sprite.setSource(m_assets->sprites["PlayerLeft"]);
        m_flames.setPosition(m_position.x - 1, m_position.y + 10, SpritePositionOffset::Center);
        break;
    case MovementDirection::Right:
        m_sprite.setSource(m_assets->sprites["PlayerRight"]);
        m_flames.setPosition(m_position.x + 1, m_position.y + 10, SpritePositionOffset::Center);
        break;
    case MovementDirection::None:
        m_sprite.setSource(m_assets->sprites["PlayerCenter"]);
        m_flames.setPosition(m_position.x, m_position.y + 10, SpritePositionOffset::Center);
        break;
    }
}
