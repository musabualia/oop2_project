#pragma once
#include "Entities/Base/Collectible.h"
#include "Core/Constants.h"
#include <box2d/box2d.h>

class Coin : public Collectible {
public:
    explicit Coin(int robotRewardValue);              // Constructor with reward value
    virtual ~Coin() = default;                        // Default destructor

    void update(float dt) override;                   // Update coin logic
    void draw(sf::RenderWindow& window) const override; // Draw coin
    void applyEffect() override;                      // Apply coin effect (grant coins)

    void createPhysicsBody(b2World* world) override {} // No physics body for coin

    void autoCollect();                               // Mark coin as auto-collected
    bool isAutoCollected() const { return m_autoCollected; } // Check if auto-collected

private:
    bool m_autoCollected = false;                     // Has the coin been auto-collected
    float m_visualTimer = 0.0f;                       // Timer for visual effect
    static constexpr float VISUAL_DURATION = 1.5f;    // Duration of visual display
};
