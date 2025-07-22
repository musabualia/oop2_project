#pragma once
#include "Entities/Base/Collectible.h"
#include "Core/Constants.h"
#include <box2d/box2d.h>

class SquadMember;

class HealthPack : public Collectible {
public:
    using HealType = GameTypes::HealType;              // Heal type alias

    explicit HealthPack(int healAmount = GameConstants::HEALTH_PACK_VALUE); // Constructor
    virtual ~HealthPack() = default;                   // Default destructor

    void update(float dt) override;                    // Update health pack state
    void draw(sf::RenderWindow& window) const override;// Draw health pack
    void applyEffect() override;                       // Apply effect when collected

    void healSquadMember(SquadMember* member);         // Heal specific squad member
    bool canHealSquadMember(const SquadMember* member) const; // Can heal the squad member

    int getHealAmount() const;                         // Get amount to heal

    void createPhysicsBody(b2World* world) override;   // Create physics body

private:
    int m_healAmount;                                  // Amount of healing provided

    float m_breatheIntensity = 0.0f;                   // Visual breathing effect intensity
    float m_breatheSpeed = 2.0f;                       // Breathing animation speed
};
