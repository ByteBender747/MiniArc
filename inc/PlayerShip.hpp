#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_time.h>
#include <cstdint>
#include <memory.h>

#include "Vector2.hpp"
#include "AppState.hpp"
#include "MiniArc.hpp"
#include "SpriteRenderer.hpp"

class PlayerShip
{
public:
    PlayerShip(sdl::AppState* state, SDL_Texture* texture);
    virtual ~PlayerShip();
private:
    enum class MovementDirection {
        None, Left, Right
    };
    struct Projectile {
        sdl::Vec2f position;
        bool isActive{false};
    };
private:
    void shipMovement();
    void animateFlames();
    void handleInputs();
    void moveAndRenderProjectiles(float shotSpeed);
    bool fireProjectile(float x, float y);
private:
    double m_speed;
    float m_posLimits[4];
    uint8_t m_flameType;
    double m_flameTimer;
    bool m_triggerState;
    std::array<Projectile, 8> m_projectiles;
    MovementDirection m_direction;
    sdl::Vec2f m_position;
    sdl::SpriteRenderer m_sprite;
    sdl::SpriteRenderer m_flames;
    sdl::SpriteRenderer m_shotSprite;
    sdl::AppState* m_app;
    GameAssets* m_assets;
};