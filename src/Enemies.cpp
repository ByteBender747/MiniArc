#include <cassert>

#include "Enemies.hpp"
#include "AnimatedSprite.hpp"
#include "AppState.hpp"
#include "MiniArc.hpp"
#include "PlayerShip.hpp"
#include "Rect.hpp"
#include "SpriteRenderer.hpp"
#include "Vector2.hpp"

// General spawning behavior
constexpr int spawnXMargin     = 8;
constexpr double spawnInterval = 2.;

// Enemy Lips behavior configuration
constexpr int lipsHitPoints              = 150;
constexpr int lipsScore                  = 200;
constexpr float lipsEnemyDownSpeed       = 40;
constexpr float lipsEnemyMaxJitter       = 32;
constexpr double lipsEnemyJitterInterval = 1.5;
constexpr float lipsProjectileSpeed      = 80;

// Enemy projectiles
constexpr int enemyProjectileDamage        = 250;
constexpr int chargedEnemyProjectileDamage = 500;

static void initBoom(GameAssets* assets, sdlc::AnimatedSprite& sprite)
{
    sprite.addFrame(assets->sprites["Boom1"]);
    sprite.addFrame(assets->sprites["Boom2"]);
    sprite.addFrame(assets->sprites["Boom3"]);
    sprite.addFrame(assets->sprites["Boom4"]);
    sprite.addFrame(assets->sprites["Boom5"]);
}

EnemySpawner::EnemySpawner(sdlc::AppState* appState)
    : m_appState(appState)
{
    assert(m_appState);
    m_appState->iterateHandler[enemyZIndex] = [this](sdlc::AppState* appState) {
        m_spawnTimer += appState->deltaTime;
        if (m_spawnTimer > spawnInterval) {
            m_spawnTimer = 0;
            auto item = m_enemies.acquire();
            if (item) {
                item->reset(new EnemyLips(appState));
            }
        }
        m_enemies.foreachAcquired([this](sdlc::PoolItem<std::unique_ptr<Enemy>>& item) {
            if (!item.value->isAlive()) {
                item.acquired = false;
            } else {
                item.value->updateAndRender();
            }
        });
    };
}

EnemySpawner::~EnemySpawner()
{
    m_appState->iterateHandler[enemyZIndex] = nullptr;
}

bool EnemySpawner::createProjectile(const sdlc::Vec2f& pos, const sdlc::Vec2f& target)
{
    auto item = m_enemies.acquire();
    if (item) {
        item->reset(new EnemyProjectile(m_appState, pos, target));
        return true;
    }
    return false;
}

bool EnemySpawner::hitCheckAllEnemies(const sdlc::Rect<float>& projectileRect, int damage)
{
    bool hitSomething = false;
    m_enemies.foreachAcquired([projectileRect, damage, &hitSomething](sdlc::PoolItem<std::unique_ptr<Enemy>>& item) {
        if (item.value->isAlive()) {
            if (projectileRect.intersects(item.value->getPositionRect())) {
                hitSomething = hitSomething || item.value->hitByProjectile(damage);
            }
        }
    });
    return hitSomething;
}

Enemy::Enemy(sdlc::AppState* appState)
    : m_appState(appState)
{
    assert(m_appState);
    m_assets = static_cast<GameAssets*>(m_appState->properties["assets"].pointer);
    assert(m_assets);
    m_viewPort = sdlc::Rect<float>(0, 0, RENDER_LOGICAL_WIDTH, RENDER_LOGICAL_HEIGHT);
}

PlayerShip* Enemy::getPlayer()
{
    PlayerShip* player = static_cast<PlayerShip*>(m_appState->properties["player"].pointer);
    assert(player);
    return player;
}

EnemyProjectile::EnemyProjectile(sdlc::AppState* appState, const sdlc::Vec2f& pos, const sdlc::Vec2f& target)
    : Enemy(appState), m_projectileSprite(m_assets->spriteTexture)
{
    m_projectileSprite.addFrame(m_assets->sprites["AlienShot11"]);
    m_projectileSprite.addFrame(m_assets->sprites["AlienShot12"]);
    m_projectileSprite.setFPS(30);
    m_projectileSprite.play();
    m_projectile.position = pos;
    m_projectile.velocity = sdlc::normalized(target - pos) * sdlc::Vec2f(lipsProjectileSpeed);
}

sdlc::Rect<float> EnemyProjectile::getPositionRect()
{
    return m_projectileSprite.destination();
}

void EnemyProjectile::updateAndRender()
{
    if (m_projectile.lifeTime > 0) {
        m_projectileSprite.setPosition(m_projectile.position.x, m_projectile.position.y);
        m_projectile.position += m_projectile.velocity * sdlc::Vec2f(m_appState->deltaTime);
        m_projectile.lifeTime -= m_appState->deltaTime;
        m_projectileSprite.update(m_appState->deltaTime);
        m_projectileSprite.render(m_appState->renderer);
        if (getPlayer()->hitCheck(m_projectileSprite.destination(), enemyProjectileDamage)) {
            kill();
        }
    } else {
        kill();
    }
}

EnemyLips::EnemyLips(sdlc::AppState* appState)
    : Enemy(appState)
    , m_sprite(m_assets->spriteTexture)
    , m_boomAnimation(m_assets->spriteTexture)
    , m_projectileSprite(m_assets->spriteTexture)
{
    m_sprite.addFrame(m_assets->sprites["Lips1"]);
    m_sprite.addFrame(m_assets->sprites["Lips2"]);
    m_sprite.addFrame(m_assets->sprites["Lips3"]);
    m_sprite.addFrame(m_assets->sprites["Lips4"]);
    m_sprite.addFrame(m_assets->sprites["Lips5"]);
    m_sprite.setPosition(spawnXMargin + (m_rng.next() % (RENDER_LOGICAL_WIDTH - spawnXMargin)), 0,
                         sdlc::SpritePositionOffset::Center);
    m_sprite.setDuration(2.5);
    m_sprite.animationFinished([this](sdlc::AnimatedSprite& ref) {
        if (getPlayer()->isAlive()) {
            EnemySpawner* spawner = static_cast<EnemySpawner*>(m_appState->properties["enemies"].pointer);
            spawner->createProjectile(m_sprite.position(), getPlayer()->getPosition());
        }
    });
    m_sprite.play();
    initBoom(m_assets, m_boomAnimation);
    m_boomAnimation.setRepeat(false);
    m_boomAnimation.setFPS(20);
    m_hitPoints = lipsHitPoints;
}

sdlc::Rect<float> EnemyLips::getPositionRect()
{
    return sdlc::Rect<float>(m_sprite.destination());
}

bool EnemyLips::hitByProjectile(int damage)
{
    m_hitPoints -= damage;
    if (m_hitPoints <= 0) {
        m_boomAnimation.play();
        m_sprite.stop();
        m_playDeathAnimation = true;
        m_appState->properties["score"].integer += lipsScore;
    } else {
        m_hitFlash = true;
    }
    return true;
}

void EnemyLips::updateAndRender()
{
    if (m_playDeathAnimation && isAlive()) {
        m_boomAnimation.setPosition(m_sprite.position().x, m_sprite.position().y, sdlc::SpritePositionOffset::Center);
        m_boomAnimation.update(m_appState->deltaTime);
        m_boomAnimation.render(m_appState->renderer);
        if (m_boomAnimation.isFinished()) kill();
        return;
    }
    sdlc::Vec2f pos(m_sprite.position());
    m_jitterTimer += m_appState->deltaTime;
    if (m_jitterTimer > lipsEnemyJitterInterval) {
        m_jitterTimer = 0;
        m_jitterValue = m_rng.normal() * lipsEnemyMaxJitter;
    }
    pos += sdlc::Vec2f(m_jitterValue, lipsEnemyDownSpeed) * sdlc::Vec2f(m_appState->deltaTime);
    m_sprite.setPosition(pos.x, pos.y, sdlc::SpritePositionOffset::Center);
    if (m_viewPort.intersects(getPositionRect())) {
        if (m_hitFlash) {
            SDL_SetRenderColorScale(m_appState->renderer, 255);
            m_hitFlash = false;
        }
        m_sprite.update(m_appState->deltaTime);
        m_sprite.render(m_appState->renderer);
        SDL_SetRenderColorScale(m_appState->renderer, 1);
    } else {
        kill(); // Runs out of visible area
    }
}

void deleteEnemies(sdlc::AppState* state, const std::string& name)
{
    EnemySpawner* ptr = static_cast<EnemySpawner*>(state->properties[name].pointer);
    if (ptr) {
        delete ptr;
        state->properties[name].pointer = nullptr;
    }
}