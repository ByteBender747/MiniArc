#pragma once

#include "Rect.hpp"
#include "Enemies.hpp"

enum class GoodyItemType
{
    powerUp, rocket, ship
};

class GoodyItem : public Enemy
{
public:
    GoodyItem(sdlc::AppState* appState, const sdlc::Point2D<float> &spawnPos, GoodyItemType type);
    void updateAndRender() override;
    bool hitByProjectile(int damage) override;
    sdlc::Rect<float> getPositionRect() override;
    void getItemSprite(sdlc::SpriteRenderer& sprite);
private:
    sdlc::Rect<float> m_posRect;
    sdlc::Vec2f m_position;
    GoodyItemType m_type;
};