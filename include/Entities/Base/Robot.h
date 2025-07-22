#pragma once
#include "Core/MovingObject.h"
#include "Core/Constants.h"
#include "Core/Timer.h"
#include <limits>

class SquadMember;

class Robot : public MovingObject {
public:
    Robot(GameTypes::RobotType type, int health, float speed, int damage);     // Constructor
    virtual ~Robot() = default;                                                // Destructor

    GameTypes::ObjectCategory getObjectCategory() const override { return GameTypes::ObjectCategory::Robot; }

    void update(float dt) override;                            // Update robot logic
    void draw(sf::RenderWindow& window) const override;        // Draw robot

    virtual void takeDamage(int damage);                       // Apply damage
    virtual void heal(int amount);                             // Heal amount
    int getHealth() const;                                     // Current health
    int getMaxHealth() const;                                  // Max health
    bool isDead() const;                                       // Is dead

    GameTypes::RobotType getType() const;                      // Robot type
    int getDamage() const;                                     // Damage value
    int getRewardValue() const;                                // Coins reward on kill

    void setLane(int lane);                                    // Set lane
    int getLane() const;                                       // Get lane
    void moveToLane(int newLane);                              // Move to another lane

    void setMovementEnabled(bool enabled) { m_movementEnabled = enabled; }  // Enable/disable movement
    bool isMovementEnabled() const { return m_movementEnabled; }

    void setFacingDirection(bool facingLeft);  // Set facing direction
    bool isFacingLeft() const { return m_facingLeft; }

    virtual void useSpecialAbility();                         // Use special ability
    virtual bool hasSpecialAbility() const { return false; }  // Has special ability
    virtual bool canUseAbility() const;                       // Can use ability
    virtual bool isPerformingSpecialAction() const { return false; }

    void setSquadMemberManager(class SquadMemberManager* manager); // Set manager
    virtual void updateSquadMemberDetection(float dt);        // Detect nearby squad members
    virtual bool shouldStopForSquadMember() const;            // Should stop for member

    bool isSquadMemberInFront(const SquadMember* member) const; // Check if member is in front
    std::vector<SquadMember*> findNearbySquadMembers() const;    // Find nearby members

    virtual void onSquadMemberCollision(SquadMember* target); // On collision
    virtual void startAttackingSquadMember(SquadMember* target); // Start attacking
    virtual void stopAttackingSquadMember();                  // Stop attacking
    virtual void updateAttackBehavior(float dt);              // Update attack

    bool isAttackingSquadMember() const { return m_isAttackingSquadMember; }    // Is attacking
    SquadMember* getTargetSquadMember() const { return m_targetSquadMember; }   // Current attack target

    virtual void updateAnimation(float dt);                   // Update animation
    virtual void switchToAttackAnimation();                   // Switch to attack anim
    virtual void switchToWalkingAnimation();                  // Switch to walk anim


protected:
    GameTypes::RobotType m_type;
    int m_health;
    int m_maxHealth;
    int m_damage;
    int m_rewardValue;
    int m_lane = 0;
    float m_originalSpeed;

    bool m_facingLeft = false;
    bool m_movementEnabled = true;

    bool m_isAttackingSquadMember = false;
    SquadMember* m_targetSquadMember = nullptr;
    Timer m_attackTimer;
    float m_attackCooldown = 1.0f;
    int m_attackDamage = 25;

    class SquadMemberManager* m_squadMemberManager = nullptr;
    float m_attackRange = 50.0f;
    bool m_hasDetectedTarget = false;

    Timer m_abilityTimer;
    float m_abilityCooldown = 5.0f;

    virtual void updateMovement(float dt);                    // Update movement
    virtual void updateCombat(float dt);                      // Update combat
    virtual void updatePhysicsMovement(float dt);             // Update physics
    virtual void handleDeathState(float dt);                  // Handle death

    virtual void processAttackTarget(float dt);               // Process attack
    virtual void executeAttack();                             // Execute attack
    virtual bool isValidAttackTarget(SquadMember* target) const; // Validate target
    virtual void cleanupInvalidTarget();                      // Cleanup invalid target

    virtual void updateDetectionState();                      // Update detection
    virtual bool checkSquadMemberInRange(const SquadMember* member) const; // Check range
    virtual float calculateDistanceToMember(const SquadMember* member) const; // Calc distance

    void loadAttackRangeFromConfig();                         // Load attack range
    void drawHealthBar(sf::RenderWindow& window) const;       // Draw health bar
};