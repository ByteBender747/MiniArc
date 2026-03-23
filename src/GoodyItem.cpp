#include "GoodyItem.hpp"

constexpr float itemDownSpeed = 50;
constexpr int pickupScore     = 120;
constexpr int shipScore       = 430;

GoodyItem::GoodyItem(sdlc::AppState *appState, const sdlc::Point2D<float> &spawnPos, GoodyItemType type)
    : Enemy(appState), m_position(spawnPos.x, spawnPos.y), m_type(type)
{
}

void GoodyItem::getItemSprite(sdlc::SpriteRenderer& sprite)
{
    switch (m_type) {
        case GoodyItemType::powerUp:
            sprite.setSource(m_assets->sprites["PowerUp2"]);
            break;
        case GoodyItemType::ship:
            sprite.setSource(m_assets->sprites["PickupS"]);
            break;
        case GoodyItemType::rocket:
            sprite.setSource(m_assets->sprites["PickupM"]);
            break;
    }
}

void GoodyItem::addScore(int points)
{
    m_appState->properties["score"].integer += points;
}

void GoodyItem::playPickupSound()
{
    m_appState->audio.stream[strmPlayerEffects].put(m_assets->pickup);
}

void GoodyItem::updateAndRender()
{
    sdlc::Rect<float> clientRect(0, 0, RENDER_LOGICAL_WIDTH, RENDER_LOGICAL_HEIGHT);
    sdlc::SpriteRenderer item(m_assets->spriteTexture);
    getItemSprite(item);
    item.setPosition(m_position.x, m_position.y);
    item.render(m_appState->renderer);
    m_posRect = sdlc::Rect<float>(item.destination());
    m_position += sdlc::Vec2f(0, 1) * sdlc::Vec2f(itemDownSpeed) * sdlc::Vec2f(m_appState->deltaTime);
    if (m_type == GoodyItemType::powerUp) {
        if (getPlayer()->hitCheck(m_posRect, -100)) {
            addScore(pickupScore);
            playPickupSound();
            kill();
        }
    }
    if (m_type == GoodyItemType::ship) {
        if (getPlayer()->hitCheck(m_posRect, 0)) {
            m_appState->properties["playerShips"].integer += 1;
            addScore(shipScore);
            playPickupSound();
            kill();
        }
    }
    if (!clientRect.intersects(m_posRect)) kill();
}

bool GoodyItem::hitByProjectile(int damage)
{
    return Enemy::hitByProjectile(damage);
}

sdlc::Rect<float> GoodyItem::getPositionRect()
{
    return m_posRect;
}
