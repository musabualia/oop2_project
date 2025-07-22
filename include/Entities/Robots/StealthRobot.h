#pragma once
#include "Entities/Base/Robot.h"
#include "Utils/ConfigLoader.h"
#include "Core/Timer.h"

class StealthRobot : public Robot {
public:
    StealthRobot();                                       // Constructor
    virtual ~StealthRobot() = default;                    // Default destructor

    void update(float dt) override;                       // Update robot logic

    void useSpecialAbility() override {}                  // No special ability
    bool hasSpecialAbility() const override { return false; } // No special ability
    bool isPerformingSpecialAction() const override;      // Check if performing special action

    void updateSquadMemberDetection(float dt) override;   // Update squad detection
    void attackSquadMember(class SquadMember* target);    // Attack a squad member

    void switchToAttackAnimation() override;              // Switch to attack animation
    void switchToWalkingAnimation() override;             // Switch to walking animation

protected:
    void initializeAnimations();                          // Initialize animations
    void updateAnimation(float dt) override;              // Update animations

private:
    bool m_deathAnimationStarted = false;                 // Is death animation started
    std::string m_currentAnimationState = "walking";      // Current animation state
    bool m_isAttacking = false;                           // Is currently attacking

    void setupFromConfig();                               // Setup from config
    ConfigLoader::RobotConfig getConfig() const;          // Get robot config

    void switchToDeathAnimation();                        // Switch to death animation
    void updateMovementBehavior(float dt);                // Update movement behavior
};
