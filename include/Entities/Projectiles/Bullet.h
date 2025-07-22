#pragma once
#include "Entities/Base/Projectile.h"
#include "Core/Constants.h"

class Robot;
class SquadMember;

class Bullet : public Projectile {
public:
    Bullet(int damage, Robot* target, SquadMember* source = nullptr);             // Target robot constructor
    Bullet(int damage, const sf::Vector2f& targetPos, SquadMember* source = nullptr); // Target position constructor

    virtual ~Bullet() = default;                                                  // Default destructor

    void update(float dt) override;                                               // Update bullet state
    void draw(sf::RenderWindow& window) const override;                           // Draw bullet
    void createPhysicsBody(b2World* world) override;                              // Create physics body

    void updateTrajectory(float dt) override;                                     // Update bullet trajectory
    void onHit() override;                                                        // Handle hit logic
    void onMiss() override;                                                       // Handle miss logic
    void applyEffects(Robot* target) override;                                    // Apply effects to robot

    static std::unique_ptr<Bullet> createStandard(int damage, Robot* target, SquadMember* source = nullptr); // Create standard bullet

private:
    void setupBulletProperties();                                                 // Setup bullet properties from config
};
