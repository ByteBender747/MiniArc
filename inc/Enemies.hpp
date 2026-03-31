#pragma once

#include <memory>

#include "AnimatedSprite.hpp"
#include "AppLayer.hpp"
#include "AppState.hpp"
#include "DynamicPool.hpp"
#include "Random.hpp"
#include "Rect.hpp"
#include "Vector2.hpp"
#include "GameConfig.hpp"

struct MiniArcGame;
struct GameAssets;
class PlayerShip;
class EnemySpawner;

class FlashModifier : public sdlc::SpriteRenderModifier
{
public:
    void renderBegin(SDL_Renderer *renderer, sdlc::SpriteRenderer &spriteRenderer, float deltaTime) override;
    void renderEnd(SDL_Renderer *renderer, sdlc::SpriteRenderer &spriteRenderer, float deltaTime) override;
    void setFlashCount(int flashCount) { m_flashFrameCount = flashCount; }
private:
    int m_flashFrameCount{0};
};

class Enemy
{
public:
    virtual ~Enemy() = default;
    Enemy(sdlc::AppState* appState);
    virtual void update() = 0;
    virtual void render(SDL_Renderer* renderer) = 0;
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
    [[nodiscard]] PlayerShip* getPlayer() const;
    [[nodiscard]] EnemySpawner* getSpawner() const;
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
    void update() override;
    void render(SDL_Renderer* renderer) override;
    bool hitByProjectile(int damage) override;
    sdlc::Rect<float> getPositionRect() override;
private:
    float m_jitterTimer{0};
    float m_jitterValue{0};
    int m_hitFlash{0};
    bool m_playDeathAnimation{false};
    sdlc::AnimatedSprite m_projectileSprite;
    sdlc::AnimatedSprite m_sprite;
    sdlc::AnimatedSprite m_boomAnimation;
};

class EnemyBon : public Enemy
{
public:
    EnemyBon(sdlc::AppState* appState);
    void update() override;
    void render(SDL_Renderer* renderer) override;
    bool hitByProjectile(int damage) override;
    sdlc::Rect<float> getPositionRect() override;
private:
    int m_posX;
    int m_posY;
    int m_hitPoints;
    int m_hitFlash{0};
    bool m_arrived{false};
    bool m_despawn{false};
    sdlc::AnimatedSprite m_explosion;
    sdlc::AnimatedSprite m_bonSprite;
};

class EnemyAlan : public Enemy
{
public:
    EnemyAlan(sdlc::AppState *appState);
    void update() override;
    void render(SDL_Renderer* renderer) override;
    bool hitByProjectile(int damage) override;
    sdlc::Rect<float> getPositionRect() override;
private:
    int m_maxDirChanges{5};
    int m_hitPoints;
    float m_actionTimer{0};
    FlashModifier m_flashModifier;
    sdlc::Vec2f m_moveDirection;
    sdlc::AnimatedSprite m_deathAnimation;
    sdlc::AnimatedSprite m_alanSprite;
};

class EnemyProjectile : public Enemy
{
public:
    EnemyProjectile(sdlc::AppState* appState, const sdlc::Vec2f& pos, const sdlc::Vec2f& dir);
    void update() override;
    void render(SDL_Renderer* renderer) override;
    sdlc::Rect<float> getPositionRect() override;
private:
    struct {
        sdlc::Vec2f position;
        sdlc::Vec2f velocity;
        float lifeTime{maxProjectileLifeTime};
    } m_projectile;
    sdlc::AnimatedSprite m_projectileSprite;
};

class EnemySpawner : public sdlc::AppLayer
{
public:
    EnemySpawner(MiniArcGame* game);
    void render(SDL_Renderer* renderer) override;
    void update(float deltaTime) override;
    bool hitCheckAllEnemies(const sdlc::Rect<float>& projectileRect, int damage);
    bool fireProjectileAtTarget(const sdlc::Vec2f& pos, const sdlc::Vec2f& target);
    bool fireProjectileAtDirection(const sdlc::Vec2f& pos, const sdlc::Vec2f& dir);
    void spawnGoody(const sdlc::Point2D<float> &spawnPos);
private:
    float m_spawnTimer{0};
    GameAssets* m_assets;
    sdlc::Random m_rng;
    sdlc::AppState* m_appState;
    sdlc::DynamicPool<std::unique_ptr<Enemy>> m_enemies;
};
