#pragma once

#include "Rect.hpp"
#include "Enemies.hpp"
#include "SpriteRenderer.hpp"

enum class GoodyItemType
{
    powerUp, rocket, ship, weaponPowerUp
};

class GoodyItem : public Enemy
{
public:
    GoodyItem(sdlc::AppState* appState, const sdlc::Point2D<float> &spawnPos, GoodyItemType type);
    void update() override;
    void render(SDL_Renderer* renderer) override;
    bool hitByProjectile(int damage) override;
    sdlc::Rect<float> getPositionRect() override;
protected:
    void addScore(int points);
    void playPickupSound();
    void getItemSprite(sdlc::SpriteRenderer& sprite);
private:
    sdlc::SpriteRenderer m_sprite;
    sdlc::Rect<float> m_posRect;
    sdlc::Vec2f m_position;
    GoodyItemType m_type;
};
