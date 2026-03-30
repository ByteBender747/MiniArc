#pragma once

#include <SDL3/SDL_render.h>

#include "AppLayer.hpp"
#include "AnimatedSprite.hpp"
#include "StaticPool.hpp"
#include "Vector2.hpp"
#include "MiniArc.hpp"
#include "SpriteRenderer.hpp"
#include "GameConfig.hpp"

class PlayerShip : public sdlc::AppLayer
{
public:
    PlayerShip(MiniArcGame* game);
    void render(SDL_Renderer *renderer) override;
    void update(float deltaTime) override;
    const sdlc::Vec2f& getPosition() const {
        return m_position;
    }
    bool isAlive() const {
        return m_isAlive;
    }
    bool isSpawning() const {
        return m_spawnEffect.playing();
    }
    int getCurrentHitPointe() const {
        return m_hitPoints;
    }
    [[nodiscard]] double getWeaponPowerUpTime() const { return m_weaponPowerUpTimer; }
    bool hitCheck(const SDL_FRect& rect, int damage);
    void enableWeaponPowerUp(float time);
private:
    enum class MovementDirection {
        None, Left, Right
    };
    struct Projectile {
        sdlc::Vec2f position;
        bool charged{false};
    };
private:
    void shipMovement();
    void animateFlames();
    void handleInputs();
    void moveAndRenderProjectiles(float shotSpeed);
    bool fireProjectile(float x, float y, bool charged);
    void reSpawn();
private:
    float m_invulnerableTimer{playerInvulnerableTime};
    float m_weaponPowerUpTimer{0};
    bool m_chargedFlag{false};
    bool m_isAlive{false};
    float m_posLimits[4]{};
    uint8_t m_flameType{0};
    double m_flameTimer{0};
    int m_hitFlash{0};
    int m_hitPoints{playerInitialHitPoints};
    double m_chargingTimer{0};
    sdlc::StaticPool<Projectile, 16> m_projectiles;
    MovementDirection m_direction{MovementDirection::None};
    sdlc::Vec2f m_position;
    sdlc::SpriteRenderer m_sprite;
    sdlc::SpriteRenderer m_flames;
    sdlc::SpriteRenderer m_shotSprite;
    sdlc::AnimatedSprite m_chargedShot;
    sdlc::AnimatedSprite m_deathAnimation;
    sdlc::AnimatedSprite m_spawnEffect;
    sdlc::AppState* m_appState;
    GameAssets* m_assets{nullptr};
};

