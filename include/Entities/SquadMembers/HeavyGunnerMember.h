#pragma once

#include "Entities/Base/SquadMember.h"
#include "Graphics/AnimationComponent.h"
#include "Core/Timer.h"
#include "Utils/ConfigLoader.h"
#include <memory>
#include <vector>

class Robot;

class HeavyGunnerMember : public SquadMember {
public:
    HeavyGunnerMember();                                        // Constructor
    virtual ~HeavyGunnerMember() = default;                     // Default destructor

    void update(float dt) override;                             // Update unit logic
    void draw(sf::RenderWindow& window) const override;         // Draw unit
    void attack(const std::vector<std::unique_ptr<Robot>>& robots) override; // Attack behavior
    Robot* findTarget(const std::vector<std::unique_ptr<Robot>>& robots) const override; // Find target

    static std::unique_ptr<HeavyGunnerMember> create() {        // Factory method
        return std::make_unique<HeavyGunnerMember>();
    }

    int getCost() const override { return m_baseCost; }         // Get unit cost
    float getRange() const override { return m_range; }         // Get unit range
    void createPhysicsBody(b2World* world) override {           // Create physics body
        createBoxPhysicsBody(world, false, 40.0f, 40.0f);
    }

    void playDeathAnimation() override;                        // Play death animation
    void setNearbyRobots(const std::vector<std::unique_ptr<Robot>>* robots); // Set visible robots

private:
    mutable AnimationComponent m_animationComponent;            // Animation component

    Timer m_attackTimer;                                        // Attack cooldown timer
    float m_attackDelay;                                        // Delay between attacks
    bool m_canFire;                                             // Can fire flag
    Robot* m_currentTarget;                                     // Current target

    enum class AnimationState {                                 // Animation states
        Idle,
        Shooting,
        Dead
    };
    AnimationState m_animationState;                            // Current animation state

    const std::vector<std::unique_ptr<Robot>>* m_visibleRobots; // Nearby visible robots

    void initializeFromConfig();                                // Load config
    void initializeAnimation();                                 // Initialize animations
    void setupAnimationCallbacks();                             // Setup animation callbacks

    void updateTargeting(float dt);                             // Update target selection
    void updateFiring(float dt);                                // Update firing logic
    void updateAnimation(float dt);                             // Update animations

    bool hasValidTarget() const;                                // Check for valid target
    void fireBullet();                                          // Fire bullet
    void startShootingAnimation();                              // Start shooting animation
    void returnToIdleAnimation();                               // Return to idle animation

    ConfigLoader::SquadMemberConfig getConfig() const;          // Get configuration
};
