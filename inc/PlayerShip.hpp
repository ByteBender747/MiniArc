#pragma once

#include <string>
#include <cstdint>
#include <memory.h>

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_time.h>

#include "AnimatedSprite.hpp"
#include "StaticPool.hpp"
#include "Vector2.hpp"
#include "AppState.hpp"
#include "MiniArc.hpp"
#include "SpriteRenderer.hpp"

constexpr int playerInitialHitPoints = 1200;

class PlayerShip
{
public:
    PlayerShip(sdlc::AppState* state, SDL_Texture* texture);
    virtual ~PlayerShip();
    const sdlc::Vec2f& getPosition() const {
        return m_position;
    }
    bool isAlive() const {
        return m_isAlive;
    }
    bool isSpawning() const {
        return m_spawnEffect.isPlaying();
    }
    int getCurrentHitPointe() const {
        return m_hitPoints;
    }
    bool hitCheck(const SDL_FRect& rect, int damage);
private:
    enum class MovementDirection {
        None, Left, Right
    };
    struct Projectile {
        sdlc::Vec2f position;
    };
private:
    void shipMovement();
    void animateFlames();
    void handleInputs();
    void moveAndRenderProjectiles(float shotSpeed);
    bool fireProjectile(float x, float y);
    void reSpawn();
    void iteratePlayerShip();
private:
    bool m_isAlive{false};
    float m_posLimits[4]{};
    uint8_t m_flameType{0};
    double m_flameTimer{0};
    bool m_triggerState{false};
    int m_hitFlash{0};
    int m_hitPoints{playerInitialHitPoints};
    sdlc::StaticPool<Projectile, 16> m_projectiles;
    MovementDirection m_direction{MovementDirection::None};
    sdlc::Vec2f m_position;
    sdlc::SpriteRenderer m_sprite;
    sdlc::SpriteRenderer m_flames;
    sdlc::SpriteRenderer m_shotSprite;
    sdlc::AnimatedSprite m_deathAnimation;
    sdlc::AnimatedSprite m_spawnEffect;
    sdlc::AppState* m_appState;
    GameAssets* m_assets;
};

void deletePlayerShip(sdlc::AppState* state, const std::string& name);