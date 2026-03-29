#include <cassert>
#include <memory>

#include "Enemies.hpp"
#include "AnimatedSprite.hpp"
#include "AppState.hpp"
#include "GoodyItem.hpp"
#include "MiniArc.hpp"
#include "PlayerShip.hpp"
#include "Rect.hpp"
#include "SpriteRenderer.hpp"
#include "Vector2.hpp"

constexpr double percentage(double x)
{
    return x / 100.;
}

// General spawning behavior
constexpr int spawnXMargin     = 8;
constexpr double spawnInterval = 2.;

// Enemy Lips behavior configuration
constexpr int lipsHitPoints              = 150;
constexpr int lipsScore                  = 200;
constexpr float lipsEnemyDownSpeed       = 40;
constexpr float lipsEnemyMaxJitter       = 32;
constexpr float lipsEnemyJitterInterval  = 1.5;
constexpr float lipsAnimationSpeed       = 1.2;

// Enemy Bon behavior configuration
constexpr int bonHitPoints            = 200;
constexpr int bonScore                = 250;
constexpr double bonSpawnProbability  = percentage(40);
constexpr double alanSpawnProbability = percentage(60);
constexpr float bonAnimationSpeed     = 3.3;

// Enemy Alan behavior configuration
constexpr float alanActionInterval   = 1.5;
constexpr float alanMoveSpeed        = 100;
constexpr int alanHitPoints          = 450;
constexpr int alanScore              = 320;
constexpr float alanAnimationSpeed   = 5;

// Enemy projectiles
constexpr float enemyProjectileSpeed       = 150;
constexpr int enemyProjectileDamage        = 250;
constexpr int chargedEnemyProjectileDamage = 500;

constexpr double spawnChancePowerUp       = percentage(30);
constexpr double spawnChanceShips         = percentage(2);
constexpr double spawnChanceWeaponPowerUp = percentage(10);

void FlashModifier::renderBegin(SDL_Renderer *renderer, sdlc::SpriteRenderer &spriteRenderer, float deltaTime)
{
    if (m_flashFrameCount > 0) {
        SDL_SetRenderColorScale(renderer, 255);
        m_flashFrameCount--;
    }
}

void FlashModifier::renderEnd(SDL_Renderer *renderer, sdlc::SpriteRenderer &spriteRenderer, float deltaTime)
{
    SDL_SetRenderColorScale(renderer, 1);
}

static void InitBoom(GameAssets* assets, sdlc::AnimatedSprite& sprite)
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
    m_assets = static_cast<GameAssets*>(m_appState->properties["assets"].pointer);
    assert(m_assets);
    m_appState->iterateHandler[enemyZIndex] = [this](sdlc::AppState* appState) {
        m_spawnTimer += appState->deltaTime;
        if (m_spawnTimer > spawnInterval) {
            m_spawnTimer = 0;
            if (auto item = m_enemies.acquire()) {
                if (m_rng.chance(bonSpawnProbability)) {
                    *item = std::make_unique<EnemyBon>(appState);
                } else if (m_rng.chance(alanSpawnProbability)) {
                    *item = std::make_unique<EnemyAlan>(appState);
                } else {
                    *item = std::make_unique<EnemyLips>(appState);
                }
            }
        }
        m_enemies.foreachAcquired([](sdlc::PoolItem<std::unique_ptr<Enemy>>& item) {
            if (!item.value->isAlive()) {
                item.acquired = false;
            } else {
                item.value->updateAndRender();
            }
        });
    };
}

void EnemySpawner::spawnGoody(const sdlc::Point2D<float> &spawnPos)
{
    if (m_rng.chance(spawnChanceShips)) {
        if (auto item = m_enemies.acquire()) {
            *item = std::make_unique<GoodyItem>(m_appState, spawnPos, GoodyItemType::ship);
            return;
        }
    }
    if (m_rng.chance(spawnChancePowerUp)) {
        if (auto item = m_enemies.acquire()) {
            *item = std::make_unique<GoodyItem>(m_appState, spawnPos, GoodyItemType::powerUp);
            return;
        }
    }
    if (m_rng.chance(spawnChanceWeaponPowerUp)) {
        if (auto item = m_enemies.acquire()) {
            *item = std::make_unique<GoodyItem>(m_appState, spawnPos, GoodyItemType::weaponPowerUp);
            return;
        }
    }
}

EnemySpawner::~EnemySpawner()
{
    m_appState->iterateHandler[enemyZIndex] = nullptr;
}

bool EnemySpawner::fireProjectileAtTarget(const sdlc::Vec2f& pos, const sdlc::Vec2f& target)
{
    auto item = m_enemies.acquire();
    if (item) {
        *item = std::make_unique<EnemyProjectile>(m_appState, pos, normalized((target - pos)));
        m_appState->audio.stream[strmAlienGun].put(m_assets->alienShot, true);
        return true;
    }
    return false;
}

bool EnemySpawner::fireProjectileAtDirection(const sdlc::Vec2f& pos, const sdlc::Vec2f& dir)
{
    auto item = m_enemies.acquire();
    if (item) {
        *item = std::make_unique<EnemyProjectile>(m_appState, pos, normalized((dir)));
        m_appState->audio.stream[strmAlienGun].put(m_assets->alienShot, true);
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

PlayerShip* Enemy::getPlayer() const
{
    auto player = static_cast<PlayerShip*>(m_appState->properties["player"].pointer);
    assert(player);
    return player;
}

EnemySpawner* Enemy::getSpawner() const
{
    auto spawner = static_cast<EnemySpawner*>(m_appState->properties["enemies"].pointer);
    assert(spawner);
    return spawner;
}

EnemyProjectile::EnemyProjectile(sdlc::AppState* appState, const sdlc::Vec2f& pos, const sdlc::Vec2f& dir)
    : Enemy(appState), m_projectileSprite(m_assets->spriteTexture)
{
    m_projectileSprite.addFrame(m_assets->sprites["AlienShot11"]);
    m_projectileSprite.addFrame(m_assets->sprites["AlienShot12"]);
    m_projectileSprite.setFPS(30);
    m_projectileSprite.play();
    m_projectile.position = pos;
    m_projectile.velocity = dir * sdlc::Vec2f(enemyProjectileSpeed);
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
        m_projectileSprite.render(m_appState->renderer, m_appState->deltaTime);
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
    m_sprite.setDuration(lipsAnimationSpeed);
    m_sprite.setCallback([this](sdlc::AnimatedSprite& ref, sdlc::AnimationEvent event) {
        if (getPlayer()->isAlive() && event == sdlc::AnimationEvent::finished) {
            EnemySpawner* spawner = static_cast<EnemySpawner*>(m_appState->properties["enemies"].pointer);
            spawner->fireProjectileAtTarget(m_sprite.position(), getPlayer()->getPosition());
        }
    });
    m_sprite.play();
    InitBoom(m_assets, m_boomAnimation);
    m_boomAnimation.setRepeat(false);
    m_boomAnimation.setFPS(20);
    m_hitPoints = lipsHitPoints;
}

sdlc::Rect<float> EnemyLips::getPositionRect()
{
    return {m_sprite.destination()};
}

bool EnemyLips::hitByProjectile(int damage)
{
    m_hitPoints -= damage;
    if (m_hitPoints <= 0 && !m_playDeathAnimation) {
        m_boomAnimation.play();
        m_sprite.stop();
        m_playDeathAnimation = true;
        m_appState->properties["score"].integer += lipsScore;
        m_appState->audio.stream[strmExplosions].put(m_assets->explosion, true);
        getSpawner()->spawnGoody(m_sprite.position());
    } else {
        m_hitFlash = 3;
    }
    return true;
}

void EnemyLips::updateAndRender()
{
    if (m_playDeathAnimation && isAlive()) {
        m_boomAnimation.setPosition(m_sprite.position().x, m_sprite.position().y, sdlc::SpritePositionOffset::Center);
        m_boomAnimation.render(m_appState->renderer, m_appState->deltaTime);
        if (m_boomAnimation.finished()) kill();
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
    if (getPlayer()->hitCheck(m_sprite.destination(), 10000)) {
        hitByProjectile(10000);
    }
    if (m_viewPort.intersects(getPositionRect())) {
        if (m_hitFlash > 0) {
            SDL_SetRenderColorScale(m_appState->renderer, 255);
            --m_hitFlash;
        }
        m_sprite.render(m_appState->renderer, m_appState->deltaTime);
        SDL_SetRenderColorScale(m_appState->renderer, 1);
    } else {
        kill(); // Runs out of visible area
    }
}

EnemyAlan::EnemyAlan(sdlc::AppState *appState)
    : Enemy(appState)
    , m_alanSprite(m_assets->spriteTexture)
    , m_deathAnimation(m_assets->spriteTexture)
    , m_hitPoints(alanHitPoints)
{
    InitBoom(m_assets, m_deathAnimation);
    m_deathAnimation.setFPS(20);
    m_deathAnimation.setRepeat(false);
    m_deathAnimation.setCallback([this](sdlc::AnimatedSprite& ref, sdlc::AnimationEvent event) {
        if (event == sdlc::AnimationEvent::finished) {
            kill();
        }
    });
    m_deathAnimation.hide();
    m_alanSprite.addFrame(m_assets->sprites["Alan1"]);
    m_alanSprite.addFrame(m_assets->sprites["Alan2"]);
    m_alanSprite.addFrame(m_assets->sprites["Alan3"]);
    m_alanSprite.addFrame(m_assets->sprites["Alan4"]);
    m_alanSprite.addFrame(m_assets->sprites["Alan5"]);
    m_alanSprite.addFrame(m_assets->sprites["Alan6"]);
    m_alanSprite.setModifier(&m_flashModifier);
    m_alanSprite.setFPS(alanAnimationSpeed);
    m_alanSprite.play();
    m_alanSprite.setPosition(
        spawnXMargin + (m_rng.next() % (RENDER_LOGICAL_WIDTH - spawnXMargin)), 0,
        sdlc::SpritePositionOffset::Center);
}

void EnemyAlan::updateAndRender()
{
    if (m_actionTimer <= 0 && m_maxDirChanges > 0) {
        m_moveDirection = sdlc::normalized(sdlc::Vec2f(getPlayer()->getPosition()) - sdlc::Vec2f(m_alanSprite.position()));
        m_actionTimer = alanActionInterval;
        --m_maxDirChanges;
    } else {
        m_actionTimer -= m_appState->deltaTime;
    }
    sdlc::Vec2f newPos = m_alanSprite.position();
    newPos += m_moveDirection * sdlc::Vec2f(alanMoveSpeed) * sdlc::Vec2f(m_appState->deltaTime);
    m_alanSprite.setPosition(newPos.x, newPos.y, sdlc::SpritePositionOffset::Center);
    if (m_viewPort.intersects(getPositionRect())) {
        m_alanSprite.render(m_appState->renderer, m_appState->deltaTime);
        m_deathAnimation.setPosition(m_alanSprite.position().x, m_alanSprite.position().y, sdlc::SpritePositionOffset::Center);
        m_deathAnimation.render(m_appState->renderer, m_appState->deltaTime);
    } else {
        kill();
    }
    if (getPlayer()->hitCheck(m_alanSprite.destination(), 10000)) {
        hitByProjectile(10000);
    }
}

bool EnemyAlan::hitByProjectile(int damage)
{
    if (!m_deathAnimation.playing()) {
        m_flashModifier.setFlashCount(3);
        m_hitPoints -= damage;
        if (m_hitPoints <= 0) {
            m_alanSprite.hide();
            m_deathAnimation.show();
            m_deathAnimation.play();
            m_appState->audio.stream[strmExplosions].put(m_assets->explosion, true);
            getSpawner()->spawnGoody(m_alanSprite.position());
        }
        return true;
    }
    return false;
}

sdlc::Rect<float> EnemyAlan::getPositionRect()
{
    return m_alanSprite.destination();
}

EnemyBon::EnemyBon(sdlc::AppState *appState)
    : Enemy(appState)
    , m_bonSprite(m_assets->spriteTexture)
    , m_explosion((m_assets->spriteTexture))
    , m_hitPoints(bonHitPoints)
{
    m_bonSprite.addFrame(m_assets->sprites["Bon1"]);
    m_bonSprite.addFrame(m_assets->sprites["Bon2"]);
    m_bonSprite.addFrame(m_assets->sprites["Bon3"]);
    m_bonSprite.addFrame(m_assets->sprites["Bon4"]);
    m_bonSprite.addFrame(m_assets->sprites["RingEffect1"]);
    m_bonSprite.addFrame(m_assets->sprites["RingEffect2"]);
    m_bonSprite.addFrame(m_assets->sprites["RingEffect3"]);
    m_bonSprite.addFrame(m_assets->sprites["RingEffect4"]);
    InitBoom(m_assets, m_explosion);
    sdlc::Rect<int> spawnArea(0, 0, RENDER_LOGICAL_WIDTH, RENDER_LOGICAL_HEIGHT);
    spawnArea = spawnArea.offset(20, 20);
    m_posX = m_rng.next() % spawnArea.width() + spawnArea.t.x;
    m_posY = m_rng.next() % spawnArea.height() + spawnArea.t.y;
    m_bonSprite.setRepeat(false);
    m_bonSprite.setDuration(bonAnimationSpeed);
    m_bonSprite.play(4);
    m_bonSprite.setCallback([this](sdlc::AnimatedSprite& ref, sdlc::AnimationEvent event) {
        if (event == sdlc::AnimationEvent::finished) {
            if (m_despawn) {
                kill();
                return;
            }
            EnemySpawner* spawner = static_cast<EnemySpawner*>(m_appState->properties["enemies"].pointer);
            if (!m_appState->properties["gameOver"].boolean) {
                sdlc::Vec2f pos = { ref.position().x, ref.position().y };
                spawner->fireProjectileAtDirection(pos, sdlc::Vec2f(-1, -1));
                spawner->fireProjectileAtDirection(pos, sdlc::Vec2f(1, -1));
                spawner->fireProjectileAtDirection(pos, sdlc::Vec2f(-1, 1));
                spawner->fireProjectileAtDirection(pos, sdlc::Vec2f(1, 1));
                spawner->fireProjectileAtDirection(pos, sdlc::Vec2f(-1, 0));
                spawner->fireProjectileAtDirection(pos, sdlc::Vec2f(1, 0));
                spawner->fireProjectileAtDirection(pos, sdlc::Vec2f(0, -1));
                spawner->fireProjectileAtDirection(pos, sdlc::Vec2f(0, 1));
            }
            m_arrived = true;
        }
    });
    m_explosion.setFPS(20);
    m_explosion.setRepeat(false);
    m_explosion.hide();
    m_explosion.setCallback([this](sdlc::AnimatedSprite& ref, sdlc::AnimationEvent event) {
        if (event == sdlc::AnimationEvent::finished) {
            kill();
            m_appState->properties["score"].integer += bonScore;
        }
    });
}

void EnemyBon::updateAndRender()
{
    if (m_hitFlash > 0) {
        SDL_SetRenderColorScale(m_appState->renderer, 255);
        --m_hitFlash;
    }
    m_bonSprite.setPosition(m_posX, m_posY, sdlc::SpritePositionOffset::Center);
    m_bonSprite.render(m_appState->renderer, m_appState->deltaTime);
    SDL_SetRenderColorScale(m_appState->renderer, 1);
    m_explosion.setPosition(m_posX, m_posY, sdlc::SpritePositionOffset::Center);
    m_explosion.render(m_appState->renderer, m_appState->deltaTime);
    if (m_bonSprite.currentFrame() >= 2 && m_bonSprite.currentFrame() <= 4) {
        if (getPlayer()->hitCheck(m_bonSprite.destination(), 10000)) {
            hitByProjectile(10000);
        }
    }
    if (m_arrived && !m_explosion.playing() && !m_despawn) {
        m_despawn = true;
        m_bonSprite.setFrame(5);
        m_bonSprite.play();
    }
}

bool EnemyBon::hitByProjectile(int damage)
{
    if (!m_despawn && !m_explosion.playing() && m_bonSprite.currentFrame() >= 2) {
        m_hitPoints -= damage;
        m_hitFlash = 3;
        if (m_hitPoints <= 0) {
            m_explosion.play();
            m_explosion.show();
            m_bonSprite.hide();
            m_appState->audio.stream[strmExplosions].put(m_assets->explosion, true);
            getSpawner()->spawnGoody(m_bonSprite.position());
        }
        return true;
    }
    return false;
}

sdlc::Rect<float> EnemyBon::getPositionRect()
{
    return m_bonSprite.destination();
}

void deleteEnemies(sdlc::AppState* state, const std::string& name)
{
    auto ptr = static_cast<EnemySpawner*>(state->properties[name].pointer);
    if (ptr) {
        delete ptr;
        state->properties[name].pointer = nullptr;
    }
}
