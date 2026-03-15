#include <cassert>

#include "Enemies.hpp"
#include "AppState.hpp"
#include "MiniArc.hpp"
#include "Rect.hpp"
#include "SpriteRenderer.hpp"
#include "StaticPool.hpp"
#include "Vector2.hpp"

// General spawning behavior
constexpr int spawnXMargin = 8;
constexpr double spawnInterval = 2.;

// Enemy Lips behavior configuration
constexpr float lipsEnemyDownSpeed = 10;
constexpr float lipsEnemyMaxJitter = 32;
constexpr double lipsEnemyJitterInterval = 1.5;

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

Enemy::Enemy(sdlc::AppState* appState) 
    : m_appState(appState)
{
    assert(m_appState);
    m_assets = static_cast<GameAssets*>(m_appState->properties["assets"].pointer);
    assert(m_assets);
    m_viewPort = sdlc::Rect<float>(0, 0, RENDER_LOGICAL_WIDTH, RENDER_LOGICAL_HEIGHT);
}

EnemyLips::EnemyLips(sdlc::AppState* appState)
    : Enemy(appState), m_sprite(m_assets->spriteTexture)
{
    m_sprite.addFrame(m_assets->sprites["Lips1"]);
    m_sprite.addFrame(m_assets->sprites["Lips2"]);
    m_sprite.addFrame(m_assets->sprites["Lips3"]);
    m_sprite.addFrame(m_assets->sprites["Lips4"]);
    m_sprite.addFrame(m_assets->sprites["Lips5"]);
    m_sprite.setPosition(spawnXMargin + (m_rng.next() % (RENDER_LOGICAL_WIDTH - spawnXMargin)), 0, sdlc::SpritePositionOffset::Center);
    m_sprite.setFPS(10);
}

void EnemyLips::updateAndRender()
{
    sdlc::Vec2f pos(m_sprite.position());
    m_jitterTimer += m_appState->deltaTime;
    if (m_jitterTimer > lipsEnemyJitterInterval) {
        m_jitterTimer = 0;
        m_jitterValue = m_rng.normal() * lipsEnemyMaxJitter;
    }
    pos += sdlc::Vec2f(m_jitterValue, lipsEnemyDownSpeed) * sdlc::Vec2f(m_appState->deltaTime);
    m_sprite.setPosition(pos.x, pos.y, sdlc::SpritePositionOffset::Center);
    sdlc::Rect<float> posRect = m_sprite.destination();
    if (m_viewPort.intersects(posRect)) {
        m_sprite.update(m_appState->deltaTime);
        m_sprite.render(m_appState->renderer);
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