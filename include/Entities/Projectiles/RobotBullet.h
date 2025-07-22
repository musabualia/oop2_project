#pragma once
#include "Entities/Base/Projectile.h"
#include "Core/Constants.h"

class Robot;
class SquadMember;

class RobotBullet : public Projectile {
public:
    RobotBullet(int damage, Robot* target, SquadMember* source = nullptr);            // Target robot constructor
    RobotBullet(int damage, const sf::Vector2f& targetPos, SquadMember* source = nullptr); // Target position constructor

    virtual ~RobotBullet() = default;                                                 // Default destructor

    GameTypes::ObjectCategory getObjectCategory() const override {
        return GameTypes::ObjectCategory::Projectile;                                 // Object category: Projectile
    }

    void update(float dt) override;                                                  // Update bullet state
    void draw(sf::RenderWindow& window) const override;                              // Draw bullet
    void createPhysicsBody(b2World* world) override;                                 // Create physics body

    void updateTrajectory(float dt) override;                                        // Update trajectory
    void onHit() override;                                                           // Handle hit
    void onMiss() override;                                                          // Handle miss
    void applyEffects(Robot* target) override;                                       // Apply effects to robot

    void applyEffectsToSquadMember(SquadMember* target);                             // Apply effects to squad member
    bool canHitSquadMembers() const override;                                        // Can this bullet hit squad members

    void setHitDistance(float distance) { m_hitDistance = distance; }                // Set hit distance

    bool isFromRobot() const { return true; }                                        // Confirm bullet is from robot
    sf::Color getBulletColor() const { return m_bulletColor; }                       // Get bullet color

    static std::unique_ptr<RobotBullet> createFromConfig(int damage, const sf::Vector2f& targetPos, Robot* source = nullptr); // Create from config

private:
    sf::Color m_bulletColor = sf::Color::Red;                                        // Bullet color
    float m_bulletSpeed = -400.0f;                                                   // Bullet speed (leftward)
    float m_bulletScale = 0.02f;                                                     // Bullet visual scale
    sf::Vector2f m_spawnOffset{ -30.0f, 0.0f };                                      // Spawn offset from robot

    float m_hitDistance = -1.0f;                                                     // Distance to hit target

    sf::RectangleShape m_bulletShape;                                               // Bullet visual shape

    void setupRobotBulletProperties();                                              // Setup properties
    void loadRobotBulletTexture();                                                  // Load texture
    void playRobotFireSound();                                                      // Play fire sound
    void playRobotImpactSound();                                                    // Play impact sound
};
