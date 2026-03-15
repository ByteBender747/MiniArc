#include "PlayerShip.hpp"
#include "AppState.hpp"
#include "MathFunc.hpp"
#include "MiniArc.hpp"
#include "SpriteRenderer.hpp"
#include "Vector2.hpp"
#include "Keyboard.hpp"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_scancode.h>

using namespace sdlc;

constexpr int playerZIndex = 1;

PlayerShip::PlayerShip(AppState* state, SDL_Texture* texture)
    : m_appState(state), m_sprite(texture), m_flames(texture), m_shotSprite(texture)
{
    m_assets = static_cast<GameAssets*>(state->properties["assets"].pointer);
    m_sprite.setScaleMode(SDL_SCALEMODE_PIXELART);
    m_position = sdlc::Vec2f(RENDER_LOGICAL_WIDTH / 2., RENDER_LOGICAL_HEIGHT - 20.);
    m_posLimits[0] = 7;
    m_posLimits[1] = RENDER_LOGICAL_WIDTH - 7;
    m_posLimits[2] = 8;
    m_posLimits[3] = RENDER_LOGICAL_HEIGHT - 8;
    m_shotSprite.setSource(m_assets->sprites["Laser"]);
    m_appState->iterateHandler[playerZIndex] = [this](AppState* appState) {
        handleInputs();
        shipMovement();
        animateFlames();
        moveAndRenderProjectiles(200);
        m_sprite.render(appState->renderer);
        m_flames.render(appState->renderer);
    };
    m_appState->eventHandler[playerZIndex] = [this](AppState* appState, SDL_Event* event) {
        if (event->type == SDL_EVENT_KEY_UP && m_triggerState) {
            if (!sdlc::getKeyState(SDL_SCANCODE_SPACE)) {
                m_triggerState = false;
            }
        }
    };
}

PlayerShip::~PlayerShip()
{
    m_appState->iterateHandler[playerZIndex] = nullptr;
    m_appState->eventHandler[playerZIndex] = nullptr;
}

bool PlayerShip::fireProjectile(float x, float y)
{
    Projectile* projectile = m_projectiles.acquire();
    if (projectile)  {
        projectile->position = sdlc::Vec2f(x, y);
    }
    return projectile != nullptr;
}

void PlayerShip::moveAndRenderProjectiles(float shotSpeed)
{
    for (auto& item : m_projectiles) {
        if (item.acquired) {
            Projectile& projectile = item.value;
            projectile.position += sdlc::Vec2f(0, -shotSpeed * m_appState->deltaTime);
            m_shotSprite.setPosition(projectile.position.x, projectile.position.y, sdlc::SpriteOrigin::Center);
            m_shotSprite.render(m_appState->renderer);
            if (projectile.position.y < 0) {
                item.acquired = false;
            }
        }
    }
}

void PlayerShip::animateFlames()
{
    m_flameTimer += m_appState->deltaTime;
    if (m_flameTimer > 0.05) {
        m_flameType = (m_flameType + 1) & 0x01;
        m_flameTimer = 0;
    }
    switch (m_direction) {
    case MovementDirection::Left:
        m_flames.setSource(m_flameType ? m_assets->sprites["FlameLeft1"] : m_assets->sprites["FlameLeft2"]);
        break;
    case MovementDirection::Right:
        m_flames.setSource(m_flameType ? m_assets->sprites["FlameRight1"] : m_assets->sprites["FlameRight2"]);
        break;
    case MovementDirection::None:
        m_flames.setSource(m_flameType ? m_assets->sprites["FlameCenter1"] : m_assets->sprites["FlameCenter2"]);
        break;
    }
}

void PlayerShip::handleInputs()
{
    float delta = m_appState->deltaTime * m_speed;
    m_direction = MovementDirection::None;
    if (m_appState->input.keys.down("shipRight")) {
        m_position += sdlc::Vec2f(delta, 0);
        m_direction = MovementDirection::Right;
    }
    if (m_appState->input.keys.down("shipLeft")) {
        m_position += sdlc::Vec2f(-delta, 0);
        m_direction = MovementDirection::Left;
    }
    if (m_appState->input.keys.down("shipUp")) {
        m_position += sdlc::Vec2f(0, -delta);
    }
    if (m_appState->input.keys.down("shipDown")) {
        m_position += sdlc::Vec2f(0, delta);
    }
    if (m_appState->input.keys.fallingEdge("fireBeam")) {
        fireProjectile(m_position.x - 4, m_position.y);
        fireProjectile(m_position.x + 4, m_position.y);
    }
}

void PlayerShip::shipMovement()
{
    m_position.x = sdlc::clamp(m_position.x, m_posLimits[0], m_posLimits[1]);
    m_position.y = sdlc::clamp(m_position.y, m_posLimits[2], m_posLimits[3]);
    m_sprite.setPosition(m_position.x, m_position.y, sdlc::SpriteOrigin::Center);
    switch (m_direction) {
    case MovementDirection::Left:
        m_sprite.setSource(m_assets->sprites["PlayerLeft"]);
        m_flames.setPosition(m_position.x - 1, m_position.y + 10, sdlc::SpriteOrigin::Center);
        break;
    case MovementDirection::Right:
        m_sprite.setSource(m_assets->sprites["PlayerRight"]);
        m_flames.setPosition(m_position.x + 1, m_position.y + 10, sdlc::SpriteOrigin::Center);
        break;
    case MovementDirection::None:
        m_sprite.setSource(m_assets->sprites["PlayerCenter"]);
        m_flames.setPosition(m_position.x, m_position.y + 10, sdlc::SpriteOrigin::Center);
        break;
    }
}

void deletePlayerShip(AppState* state, const std::string& name)
{
    PlayerShip* player = static_cast<PlayerShip*>(state->properties[name].pointer);
    if (player) {
        delete player;
        state->properties[name].pointer = nullptr;
    }
}