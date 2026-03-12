#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_time.h>
#include <cstdint>
#include <memory.h>

#include "Vector2.hpp"
#include "AppEntry.hpp"
#include "MiniArc.hpp"
#include "SpriteRenderer.hpp"

class PlayerShip
{
public:
    PlayerShip(AppState* state, SDL_Texture* texture);
    void update();
    void render();
private:
    void shipMovement();
    void animateFlames();
private:
    enum class MovementDirection {
        None, Left, Right
    };
    double m_speed;
    float m_posLimits[4];
    uint8_t m_flameType;
    double m_flameTimer;
    MovementDirection m_direction;
    sdl::Vec2f m_position;
    sdl::SpriteRenderer m_sprite;
    sdl::SpriteRenderer m_flames;
    AppState* m_app;
    GameState* m_game;
};