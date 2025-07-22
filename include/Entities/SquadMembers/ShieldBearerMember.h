#pragma once

#include "Entities/Base/SquadMember.h"
#include "Graphics/AnimationComponent.h"
#include "Core/Timer.h"
#include "Utils/ConfigLoader.h"
#include <memory>
#include <vector>

class Robot;

class ShieldBearerMember : public SquadMember {
public:
    ShieldBearerMember();                                      // Constructor
    virtual ~ShieldBearerMember() = default;                   // Default destructor

    void update(float dt) override;                            // Update logic
    void draw(sf::RenderWindow& window) const override;        // Draw unit
    void attack(const std::vector<std::unique_ptr<Robot>>& robots) override; // Attack behavior
    Robot* findTarget(const std::vector<std::unique_ptr<Robot>>& robots) const override; // Find target

    static std::unique_ptr<ShieldBearerMember> create() {      // Factory method
        return std::make_unique<ShieldBearerMember>();
    }

    int getCost() const override { return m_baseCost; }        // Get unit cost
    float getRange() const override { return m_range; }        // Get unit range
    void createPhysicsBody(b2World* world) override {          // Create physics body
        createBoxPhysicsBody(world, false, 40.0f, 40.0f);
    }

    void playDeathAnimation() override;                       // Play death animation
    void setNearbyRobots(const std::vector<std::unique_ptr<Robot>>* robots); // Set nearby robots

private:
    mutable AnimationComponent m_animationComponent;           // Animation component

    enum class AnimationState {                                // Animation states
        Idle,
        Blocking,
        Dead
    };
    AnimationState m_animationState;                           // Current animation state

    const std::vector<std::unique_ptr<Robot>>* m_visibleRobots = nullptr; // Visible robots
    bool m_robotNear = false;                                  // Is robot nearby

    void initializeFromConfig();                               // Load config
    void initializeAnimation();                                // Setup animations
    void setupAnimationCallbacks();                            // Setup animation callbacks

    void updateRobotDetection(float dt);                       // Update robot detection
    void updateAnimation(float dt);                            // Update animation state

    void switchToBlockingAnimation();                          // Switch to blocking animation
    void returnToIdleAnimation();                              // Return to idle animation

    ConfigLoader::SquadMemberConfig getConfig() const;         // Get configuration
};
