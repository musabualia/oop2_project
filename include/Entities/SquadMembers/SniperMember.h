#pragma once

#include "Entities/Base/SquadMember.h"
#include "Graphics/AnimationComponent.h"
#include "Core/Timer.h"
#include "Utils/ConfigLoader.h"
#include <memory>
#include <vector>

class Robot;

class SniperMember : public SquadMember {
public:
    SniperMember();                                          // Constructor
    virtual ~SniperMember() = default;                       // Default destructor

    void update(float dt) override;                          // Update unit logic
    void draw(sf::RenderWindow& window) const override;      // Draw unit
    void attack(const std::vector<std::unique_ptr<Robot>>& robots) override; // Attack behavior
    Robot* findTarget(const std::vector<std::unique_ptr<Robot>>& robots) const override; // Find target

    static std::unique_ptr<SniperMember> create() {          // Factory method
        return std::make_unique<SniperMember>();
    }

    int getCost() const override { return m_baseCost; }      // Get unit cost
    float getRange() const override { return m_range; }      // Get unit range
    void createPhysicsBody(b2World* world) override {        // Create physics body
        createBoxPhysicsBody(world, false, 40.0f, 40.0f);
    }

    void playDeathAnimation() override;                     // Play death animation
    void setNearbyRobots(const std::vector<std::unique_ptr<Robot>>* robots); // Set visible robots

private:
    mutable AnimationComponent m_animationComponent;         // Animation component

    Timer m_attackTimer;                                     // Timer for attacks
    float m_attackDelay;                                     // Delay between attacks
    bool m_canFire;                                          // Can fire flag
    Robot* m_currentTarget;                                  // Current target

    enum class AnimationState {                              // Animation states
        Idle,
        Shooting,
        Dead
    };
    AnimationState m_animationState;                         // Current animation state

    const std::vector<std::unique_ptr<Robot>>* m_visibleRobots; // Visible robots

    void initializeFromConfig();                             // Load from config
    void initializeAnimation();                              // Initialize animations
    void setupAnimationCallbacks();                          // Setup animation callbacks

    void updateTargeting(float dt);                          // Update target selection
    void updateFiring(float dt);                             // Update firing behavior
    void updateAnimation(float dt);                          // Update animation state

    bool hasValidTarget() const;                             // Check if current target is valid
    void fireBullet();                                       // Fire a bullet
    void startShootingAnimation();                           // Start shooting animation
    void returnToIdleAnimation();                            // Return to idle animation

    ConfigLoader::SquadMemberConfig getConfig() const;       // Get configuration
};
