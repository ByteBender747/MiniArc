#include "PlayerShip.hpp"
#include "AppEntry.hpp"
#include "MathFunc.hpp"
#include "MiniArc.hpp"
#include "SpriteRenderer.hpp"
#include "Vector2.hpp"
#include "Keyboard.hpp"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_scancode.h>

constexpr int playerZIndex = 1;

PlayerShip::PlayerShip(AppState* state, SDL_Texture* texture)
    : m_app(state), m_sprite(texture), m_flames(texture), m_shotSprite(texture),
      m_speed(100), m_direction(MovementDirection::None), m_flameType(0), m_flameTimer(0), m_triggerState(false)
{
    m_game = static_cast<GameState*>(state->userdata);
    m_sprite.setScaleMode(SDL_SCALEMODE_PIXELART);
    m_position = sdl::Vec2f(RENDER_LOGICAL_WIDTH / 2., RENDER_LOGICAL_HEIGHT - 20.);
    m_posLimits[0] = 7;
    m_posLimits[1] = RENDER_LOGICAL_WIDTH - 7;
    m_posLimits[2] = 8;
    m_posLimits[3] = RENDER_LOGICAL_HEIGHT - 8;
    m_shotSprite.setSource(m_game->sprites["Laser"]);
    m_app->iterateHandler[playerZIndex] = [this](AppState* appState) {
        handleInputs();
        shipMovement();
        animateFlames();
        moveAndRenderProjectiles(200);
        m_sprite.render(appState->renderer);
        m_flames.render(appState->renderer);
    };
    m_app->eventHandler[playerZIndex] = [this](AppState* appState, SDL_Event* event) {
        if (event->type == SDL_EVENT_KEY_UP && m_triggerState) {
            if (!sdl::KeyPressed(SDL_SCANCODE_SPACE)) {
                m_triggerState = false;
            }
        }
    };
}

PlayerShip::~PlayerShip()
{
    m_app->iterateHandler[playerZIndex] = nullptr;
    m_app->eventHandler[playerZIndex] = nullptr;
}

bool PlayerShip::fireProjectile(float x, float y)
{
    for (auto& projectile : m_projectiles) {
        if (!projectile.isActive) {
            projectile.position = sdl::Vec2f(x, y);
            projectile.isActive = true;
            return true;
        }
    }
    return false;
}

void PlayerShip::moveAndRenderProjectiles(float shotSpeed)
{
    for (auto& projectile : m_projectiles) {
        if (projectile.isActive) {
            projectile.position += sdl::Vec2f(0, -shotSpeed * m_app->deltaTime);
            m_shotSprite.setPosition(projectile.position.x, projectile.position.y, sdl::SpriteOrigin::Center);
            m_shotSprite.render(m_app->renderer);
            if (projectile.position.y < 0) {
                projectile.isActive = false;
            }
        }
    }
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

void PlayerShip::handleInputs()
{
    float delta = m_app->deltaTime * m_speed;
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
    if (sdl::KeyPressed(SDL_SCANCODE_SPACE) && !m_triggerState) {
        fireProjectile(m_position.x, m_position.y);
        m_triggerState = true;
    }
}

void PlayerShip::shipMovement()
{
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