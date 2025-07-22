#pragma once
#include "Entities/Base/Projectile.h"
#include "Core/Constants.h"

class Robot;
class SquadMember;

class SniperBullet : public Projectile {
public:
    SniperBullet(int damage, Robot* target, SquadMember* source = nullptr);             // Target robot constructor
    SniperBullet(int damage, const sf::Vector2f& targetPos, SquadMember* source = nullptr); // Target position constructor

    virtual ~SniperBullet() = default;                                                  // Default destructor

    void update(float dt) override;                                                     // Update sniper bullet
    void draw(sf::RenderWindow& window) const override;                                 // Draw sniper bullet
    void createPhysicsBody(b2World* world) override;                                    // Create physics body

    void updateTrajectory(float dt) override;                                           // Update bullet trajectory
    void onHit() override;                                                              // Handle hit logic
    void onMiss() override;                                                             // Handle miss logic
    void applyEffects(Robot* target) override;                                          // Apply effects to robot

    static std::unique_ptr<SniperBullet> createStandard(int damage, Robot* target, SquadMember* source = nullptr); // Create standard sniper bullet

private:
    void setupSniperBulletProperties();                                                 // Setup sniper bullet properties
};
