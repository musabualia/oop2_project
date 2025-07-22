#pragma once
#include "Core/StaticObject.h"
#include "Core/Constants.h"
#include "Core/Timer.h"
#include <vector>
#include <memory>

class Robot;

class SquadMember : public StaticObject {
public:
    SquadMember(GameTypes::SquadMemberType type, int cost, float range, int damage);  // Constructor
    virtual ~SquadMember();                                                           // Destructor

    void removeAttacker(Robot* attacker);              // Remove a robot from attackers list

    GameTypes::ObjectCategory getObjectCategory() const override { 
        return GameTypes::ObjectCategory::SquadMember; // Object category: SquadMember
    }

    void update(float dt) override;                    // Update squad member
    void draw(sf::RenderWindow& window) const override;// Draw squad member

    virtual void attack(const std::vector<std::unique_ptr<Robot>>& robots) = 0; // Attack logic (pure virtual)
    virtual Robot* findTarget(const std::vector<std::unique_ptr<Robot>>& robots) const; // Find target robot

    GameTypes::SquadMemberType getType() const;        // Get unit type
    virtual int getCost() const;                       // Get unit cost
    int getBaseCost() const;                           // Get base cost
    virtual float getRange() const;                    // Get attack range
    int getDamage() const;                             // Get damage

    virtual void playDeathAnimation();                 // Play death animation

    bool canAttack() const;                            // Can attack now
    void resetAttackTimer();                           // Reset attack timer
    float getAttackCooldown() const;                   // Get attack cooldown

    void setTargetPriority(GameTypes::TargetPriority priority); // Set targeting priority
    GameTypes::TargetPriority getTargetPriority() const;        // Get targeting priority
    Robot* getCurrentTarget() const;                   // Get current target

    virtual void setNearbyRobots(const std::vector<std::unique_ptr<Robot>>* robots) {} // Store nearby robots (optional)

    virtual void takeDamage(int damage);               // Take damage
    virtual void heal(int amount);                     // Heal by fixed amount
    void healByPercentage(float percentage);           // Heal by percentage
    bool canBeHealed() const;                          // Can be healed
    int getHealth() const;                             // Get current health
    int getMaxHealth() const;                          // Get max health
    bool isDestroyed() const;                          // Check if destroyed

    bool canBePlacedAt(int lane, int gridX) const override; // Validate placement

    virtual void onRobotCollision(Robot* attacker);    // Handle collision with robot
    virtual void onRobotBulletHit(int damage);         // Handle hit by bullet

protected:
    GameTypes::SquadMemberType m_type;                 // Unit type
    int m_baseCost;                                    // Base cost of unit
    int m_damage;                                      // Damage per hit
    float m_range;                                     // Attack range
    float m_attackCooldown;                            // Cooldown between attacks

    Timer m_attackTimer;                               // Timer for attack cooldown
    Robot* m_currentTarget = nullptr;                  // Current target robot
    GameTypes::TargetPriority m_targetPriority = GameTypes::TargetPriority::Closest; // Targeting priority

    int m_health = 100;                                // Current health
    int m_maxHealth = 100;                             // Max health

    bool m_isDying = false;                            // Is in dying state
    float m_deathAnimTimer = 0.f;                      // Timer for death animation
    float m_deathAnimDuration = 1.0f;                  // Duration of death animation

    Timer m_abilityTimer;                              // Timer for abilities
    float m_abilityCooldown = 10.0f;                   // Cooldown for abilities

    std::vector<Robot*> m_attackingRobots;             // List of attacking robots
    bool m_underAttack = false;                        // Under attack flag

    void initializeConfigFromName(const std::string& configName, float defaultAttackSpeed = 1.5f); // Load config

    virtual void updateTargeting(const std::vector<std::unique_ptr<Robot>>& robots); // Update target selection
    virtual void updateAttack(float dt);                 // Update attack logic

    float distanceToTarget(const Robot* robot) const;    // Distance to a robot
    bool isInRange(const Robot* robot) const;            // Check if robot is in range
    bool isValidTarget(const Robot* robot) const;        // Validate if robot is a valid target

private:
    void drawHealthBar(sf::RenderWindow& window) const;  // Draw health bar
};
