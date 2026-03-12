#include "PlayerShip.hpp"
#include "AppEntry.hpp"
#include "MathFunc.hpp"
#include "MiniArc.hpp"
#include "SpriteRenderer.hpp"
#include "Vector2.hpp"
#include "Keyboard.hpp"
#include <SDL3/SDL_scancode.h>

PlayerShip::PlayerShip(AppState* state, SDL_Texture* texture)
    : m_app(state), m_sprite(texture), m_flames(texture),
      m_speed(70), m_direction(MovementDirection::None), m_flameType(0), m_flameTimer(0)
{
    m_game = static_cast<GameState*>(state->userdata);
    m_sprite.setScaleMode(SDL_SCALEMODE_PIXELART);
    m_position = sdl::Vec2f(RENDER_LOGICAL_WIDTH / 2., RENDER_LOGICAL_HEIGHT - 20.);
    m_posLimits[0] = 7;
    m_posLimits[1] = RENDER_LOGICAL_WIDTH - 7;
    m_posLimits[2] = 8;
    m_posLimits[3] = RENDER_LOGICAL_HEIGHT - 8;
}

void PlayerShip::render()
{
    m_sprite.render(m_app->renderer);
    m_flames.render(m_app->renderer);
}

void PlayerShip::animateFlames()
{
    m_flameTimer += m_app->deltaTime;
    if (m_flameTimer > 0.05) {
        m_flameType = (m_flameType + 1) & 0x01;
        m_flameTimer = 0;
    }
    switch (m_direction) {
    case MovementDirection::Left:
        m_flames.setSource(m_flameType ? m_game->sprites["FlameLeft1"] : m_game->sprites["FlameLeft2"]);
        break;
    case MovementDirection::Right:
        m_flames.setSource(m_flameType ? m_game->sprites["FlameRight1"] : m_game->sprites["FlameRight2"]);
        break;
    case MovementDirection::None:
        m_flames.setSource(m_flameType ? m_game->sprites["FlameCenter1"] : m_game->sprites["FlameCenter2"]);
        break;
    }
}

void PlayerShip::shipMovement()
{
    sdl::Vec2f oldPos = m_position;
    float delta = m_app->deltaTime * m_speed;
    m_sprite.setSource(m_game->sprites["PlayerCenter"]);
    m_direction = MovementDirection::None;
    if (sdl::KeyPressed(SDL_SCANCODE_D)) {
        m_position += sdl::Vec2f(delta, 0);
        m_direction = MovementDirection::Right;
    }
    if (sdl::KeyPressed(SDL_SCANCODE_A)) {
        m_position += sdl::Vec2f(-delta, 0);
        m_direction = MovementDirection::Left;
    }
    if (sdl::KeyPressed(SDL_SCANCODE_W)) {
        m_position += sdl::Vec2f(0, -delta);
    }
    if (sdl::KeyPressed(SDL_SCANCODE_S)) {
        m_position += sdl::Vec2f(0, delta);
    }
    m_position.x = sdl::clamp(m_position.x, m_posLimits[0], m_posLimits[1]);
    m_position.y = sdl::clamp(m_position.y, m_posLimits[2], m_posLimits[3]);
    m_sprite.setPosition(m_position.x, m_position.y, sdl::SpriteOrigin::Center);
    switch (m_direction) {
    case MovementDirection::Left:
        m_sprite.setSource(m_game->sprites["PlayerLeft"]);
        m_flames.setPosition(m_position.x - 1, m_position.y + 10, sdl::SpriteOrigin::Center);
        break;
    case MovementDirection::Right:
        m_sprite.setSource(m_game->sprites["PlayerRight"]);
        m_flames.setPosition(m_position.x + 1, m_position.y + 10, sdl::SpriteOrigin::Center);
        break;
    case MovementDirection::None:
        m_sprite.setSource(m_game->sprites["PlayerCenter"]);
        m_flames.setPosition(m_position.x, m_position.y + 10, sdl::SpriteOrigin::Center);
        break;
    }
}

void PlayerShip::update()
{
    shipMovement();
    animateFlames();
}