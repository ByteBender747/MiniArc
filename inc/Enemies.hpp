#pragma once

#include <memory>

#include "AnimatedSprite.hpp"
#include "AppState.hpp"
#include "DynamicPool.hpp"
#include "MiniArc.hpp"
#include "PlayerShip.hpp"
#include "Random.hpp"
#include "Rect.hpp"
#include "Vector2.hpp"

constexpr double maxProjectileLifeTime = 5;

class Enemy
{
public:
    virtual ~Enemy() = default;
    Enemy(sdlc::AppState* appState);
    virtual void updateAndRender() = 0;
    virtual bool hitByProjectile(int damage) { return false; };
    virtual sdlc::Rect<float> getPositionRect() = 0;
    void kill() {
        m_alive = false;
    }
    bool isAlive() const {
        return m_alive;
    }
public:
    enum EnemyType {
        None, Alan, Lips, Bon
    };
protected:
    PlayerShip* getPlayer() const;
protected:
    int m_hitPoints{1};
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
    bool hitByProjectile(int damage) override;
    sdlc::Rect<float> getPositionRect() override;
private:
    double m_jitterTimer{0};
    float m_jitterValue{0};
    int m_hitFlash{0};
    bool m_playDeathAnimation{false};
    sdlc::AnimatedSprite m_projectileSprite;
    sdlc::AnimatedSprite m_sprite;
    sdlc::AnimatedSprite m_boomAnimation;
};

class EnemyProjectile : public Enemy
{
public:
    EnemyProjectile(sdlc::AppState* appState, const sdlc::Vec2f& pos, const sdlc::Vec2f& target);
    void updateAndRender() override;
    sdlc::Rect<float> getPositionRect() override;
private:
    struct {
        sdlc::Vec2f position;
        sdlc::Vec2f velocity;
        double lifeTime{maxProjectileLifeTime};
    } m_projectile;
    sdlc::AnimatedSprite m_projectileSprite;
};

class EnemySpawner
{
public:
    EnemySpawner(sdlc::AppState* appState);
    virtual ~EnemySpawner();
    bool hitCheckAllEnemies(const sdlc::Rect<float>& projectileRect, int damage);
    bool createProjectile(const sdlc::Vec2f& pos, const sdlc::Vec2f& target);
private:
    double m_spawnTimer{0};
    GameAssets* m_assets;
    sdlc::AppState* m_appState;
    sdlc::DynamicPool<std::unique_ptr<Enemy>> m_enemies;
};

void deleteEnemies(sdlc::AppState* state, const std::string& name);