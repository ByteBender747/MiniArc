#pragma once

constexpr double percentage(double x)
{
    return x / 100.;
}

// Audio stream assignment
constexpr int strmPlayerGun = 0;
constexpr int strmAlienGun = 1;
constexpr int strmExplosions = 2;
constexpr int strmPlayerEffects = 3;

// Drawing order configuration
constexpr int backgroundZIndex = 0;
constexpr int enemyZIndex = 1;
constexpr int playerZIndex = 2;
constexpr int uiZIndex = 3;
constexpr int textInputSlot = 4;

// Player configuration
constexpr float playerMoveSpeed        = 100;
constexpr int playerBeamDamage         = 50;
constexpr int playerChargedBeamDamage  = 200;
constexpr float playerInvulnerableTime = 1;
constexpr int playerInitialHitPoints   = 1200;

// General spawning behavior
constexpr int spawnXMargin     = 8;
constexpr double spawnInterval = 2.;

// Enemy Lips behavior configuration
constexpr int lipsHitPoints              = 150;
constexpr int lipsScore                  = 200;
constexpr float lipsEnemyDownSpeed       = 40;
constexpr float lipsEnemyMaxJitter       = 32;
constexpr float lipsEnemyJitterInterval  = 1.5;
constexpr float lipsAnimationSpeed       = 1.2;

// Enemy Bon behavior configuration
constexpr int bonHitPoints            = 200;
constexpr int bonScore                = 250;
constexpr double bonSpawnProbability  = percentage(40);
constexpr double alanSpawnProbability = percentage(60);
constexpr float bonAnimationSpeed     = 3.3;

// Enemy Alan behavior configuration
constexpr float alanActionInterval   = 1.5;
constexpr float alanMoveSpeed        = 100;
constexpr int alanHitPoints          = 450;
constexpr int alanScore              = 320;
constexpr float alanAnimationSpeed   = 5;

// Enemy projectiles
constexpr float enemyProjectileSpeed       = 150;
constexpr int enemyProjectileDamage        = 250;
constexpr int chargedEnemyProjectileDamage = 500;
constexpr double maxProjectileLifeTime     = 5;

// Enemy spawning probabilities
constexpr double spawnChancePowerUp       = percentage(30);
constexpr double spawnChanceShips         = percentage(2);
constexpr double spawnChanceWeaponPowerUp = percentage(10);
