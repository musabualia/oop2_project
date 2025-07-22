#pragma once
#include "Entities/Base/Robot.h"
#include "Utils/ConfigLoader.h"
#include "Core/Timer.h"

class FireRobot : public Robot {
public:
    FireRobot();                                        // Constructor
    virtual ~FireRobot() = default;                     // Default destructor

    void update(float dt) override;                     // Update robot logic

    void useSpecialAbility() override {}                // No special ability
    bool hasSpecialAbility() const override { return false; } // No special ability
    bool isPerformingSpecialAction() const override;    // Check if performing action

    void updateSquadMemberDetection(float dt) override; // Update detection logic
    void switchToAttackAnimation() override;            // Switch to attack animation
    void switchToWalkingAnimation() override;           // Switch to walking animation

    void startShooting(class SquadMember* target);      // Start shooting at target
    void stopShooting();                                // Stop shooting
    void fireBullet();                                  // Fire a bullet
    bool isReadyToFire() const;                         // Check if ready to fire

protected:
    void initializeAnimations();                        // Initialize animations
    void updateAnimation(float dt) override;            // Update animation state

private:
    bool m_isShooting = false;                          // Is currently shooting
    bool m_hasTargetInRange = false;                    // Is target in range
    class SquadMember* m_shootingTarget = nullptr;      // Current shooting target

    Timer m_bulletTimer;                                // Bullet cooldown timer
    float m_bulletCooldown = 2.0f;                      // Time between shots
    int m_bulletDamage = 50;                            // Bullet damage

    bool m_deathAnimationStarted = false;               // Is death animation started
    std::string m_currentAnimationState = "walking";    // Current animation state

    void setupFromConfig();                             // Load settings from config
    ConfigLoader::RobotConfig getConfig() const;        // Get config struct

    void switchToDeathAnimation();                      // Switch to death animation
    void updateBulletFiring(float dt);                  // Update bullet firing logic
    bool canFireBullet() const;                         // Check if can fire bullet
    sf::Vector2f getBulletSpawnPosition() const;        // Get position to spawn bullet

    void updateMovementBehavior(float dt);              // Update movement behavior
    void updateShootingBehavior(float dt);              // Update shooting behavior
};
