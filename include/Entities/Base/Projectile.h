#pragma once
#include "Core/MovingObject.h"
#include "Core/Constants.h"
#include "Core/Timer.h"

class Robot;
class SquadMember;

class Projectile : public MovingObject {
public:
    Projectile(GameTypes::ProjectileType type, int damage, float speed, Robot* target, SquadMember* source = nullptr); // Targeting robot
    Projectile(GameTypes::ProjectileType type, int damage, float speed, const sf::Vector2f& targetPos, SquadMember* source = nullptr); // Targeting position

    virtual ~Projectile() = default;

    GameTypes::ObjectCategory getObjectCategory() const override { return GameTypes::ObjectCategory::Projectile; }

    void update(float dt) override;                           // Update projectile state
    void draw(sf::RenderWindow& window) const override;       // Render projectile
    void createPhysicsBody(b2World* world) override;          // Create physics body

    GameTypes::ProjectileType getType() const;                // Get projectile type
    int getDamage() const;                                    // Get projectile damage
    bool shouldRemove() const;                                // Should projectile be removed

    Robot* getTargetRobot() const;                            // Get targeted robot
    sf::Vector2f getTargetPosition() const;                   // Get target position
    SquadMember* getSource() const;                           // Get source squad member
    GameTypes::ProjectileTargetType getTargetType() const;    // Get target type

    virtual bool hasHitTarget() const;                        // Check if target hit
    virtual void onHit();                                     // Handle hit event
    virtual void onMiss();                                    // Handle miss event

    virtual void updateTrajectory(float dt);                  // Update trajectory
    virtual void applyEffects(Robot* target);                 // Apply effect to robot

    virtual void applyEffectsToSquadMember(SquadMember* target); // Apply effect to squad member
    virtual bool canHitSquadMembers() const { return false; }    // Can hit squad members

    bool isExpired() const;                                   // Is projectile expired
    float getLifetime() const;                                // Get total lifetime
    float getRemainingLifetime() const;                       // Get remaining lifetime

protected:
    GameTypes::ProjectileType m_type;                         // Projectile type
    int m_damage;                                             // Damage value
    GameTypes::ProjectileTargetType m_targetType;             // Type of target

    Robot* m_targetRobot = nullptr;                           // Robot target
    sf::Vector2f m_targetPosition;                            // Position target
    SquadMember* m_source = nullptr;                          // Source unit

    Timer m_lifetimeTimer;                                    // Timer for lifetime
    bool m_shouldRemove = false;                              // Flag for removal
    bool m_hasHit = false;                                    // Hit status

    float m_travelDistance = 0.0f;                            // Distance traveled
    float m_maxRange = 1000.0f;                               // Max range

    virtual void checkLifetime(float dt);                     // Check expiration
    bool isTargetValid() const;                               // Validate target
    float distanceToTarget() const;                           // Distance to target
};
