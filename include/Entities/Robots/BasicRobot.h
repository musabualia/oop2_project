#pragma once
#include "Entities/Base/Robot.h"
#include "Utils/ConfigLoader.h"

class BasicRobot : public Robot {
public:
    BasicRobot();                                         // Constructor
    virtual ~BasicRobot() = default;                      // Default destructor

    void useSpecialAbility() override {}                 // No special ability
    bool hasSpecialAbility() const override { return false; } // BasicRobot has no abilities
    bool isPerformingSpecialAction() const override;     // Check if performing special action

    void switchToAttackAnimation() override;             // Switch to attack animation
    void switchToWalkingAnimation() override;            // Switch to walking animation
    void updateSquadMemberDetection(float dt) override;  // Update squad detection

protected:
    void initializeAnimations();                         // Initialize animations
    void updateAnimation(float dt) override;             // Update animations

private:
    bool m_deathAnimationStarted = false;                // Death animation state
    void setupFromConfig();                              // Load properties from config
};