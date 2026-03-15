#pragma once

#include "AnimatedSprite.hpp"
#include "AppState.hpp"
#include "MiniArc.hpp"
#include "Random.hpp"
#include "Rect.hpp"
#include "StaticPool.hpp"
#include <memory>

class Enemy
{
public:
    Enemy(sdlc::AppState* appState);
    virtual void updateAndRender() = 0;
    void kill() { m_alive = false; }
    bool isAlive() const { return m_alive; }
public:
    enum EnemyType {
        None, Alan, Lips, Bon
    };
protected:
    GameAssets* m_assets;
    EnemyType m_type{None};
    sdlc::Random m_rng;
    sdlc::AppState* m_appState;
    sdlc::Rect<float> m_viewPort;
private:
    bool m_alive{true};
};

class EnemyLips : public Enemy
{
public:
    EnemyLips(sdlc::AppState* appState);
    void updateAndRender() override;
private:
    double m_jitterTimer{0};
    float m_jitterValue{0};
    sdlc::AnimatedSprite m_sprite;
};

class EnemySpawner
{
public:
    EnemySpawner(sdlc::AppState* appState);
    virtual ~EnemySpawner();
private:
    double m_spawnTimer{0};
    sdlc::AppState* m_appState;
    sdlc::StaticPool<std::unique_ptr<Enemy>, 8> m_enemies;
};

void deleteEnemies(sdlc::AppState* state, const std::string& name);