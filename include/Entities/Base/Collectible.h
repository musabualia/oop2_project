#pragma once
#include "Core/StaticObject.h"
#include "Core/Constants.h"
#include "Core/Timer.h"
#include <SFML/Graphics.hpp>

class Collectible : public StaticObject {
public:
    Collectible(GameTypes::CollectibleType type, int value, float duration = 0.0f); // Constructor
    virtual ~Collectible() = default;                                               // Virtual destructor

    GameTypes::ObjectCategory getObjectCategory() const override { return GameTypes::ObjectCategory::Collectible; }

    void update(float dt) override;                          // Update collectible state
    void draw(sf::RenderWindow& window) const override;      // Draw collectible

    GameTypes::CollectibleType getType() const;              // Get collectible type
    int getValue() const;                                    // Get point/heal value
    bool isCollected() const;                                // Check if collected
    bool isExpired() const;                                  // Check if expired

    virtual void collect();                                  // Trigger collection
    virtual void applyEffect() = 0;                           // Apply collectible effect

    void spawn(const sf::Vector2f& position);                // Set initial position
    float getLifetime() const;                               // Get lifetime duration
    float getRemainingLifetime() const;                      // Time left before expiration

    float getCollectionRadius() const;                       // Get pickup radius
    void setCollectionRadius(float radius);                  // Set pickup radius

protected:
    GameTypes::CollectibleType m_type;                       // Type of collectible
    int m_value;                                              // Value (coins/heal)
    bool m_collected = false;                                 // Collected state

    Timer m_lifetimeTimer;                                    // Timer for lifetime
    float m_maxLifetime = GameConstants::COLLECTIBLE_LIFETIME;

    float m_collectionRadius = GameConstants::COLLECTION_RADIUS; // Auto-collect radius

    virtual void checkExpiration(float dt);                  // Check expiration state
};
