#pragma once
#include <SFML/Graphics.hpp>
#include "Graphics/AnimationComponent.h"
#include <memory>

class Bomb {
public:
    Bomb(const sf::Vector2f& pos);                     // Constructor with position

    void update(float dt);                             // Update bomb state
    void render(sf::RenderWindow& window) const;       // Render bomb
    bool isDone() const;                               // Check if bomb is finished

    void explode();                                    // Trigger explosion
    bool hasExploded() const;                          // Check if bomb has exploded

    sf::Vector2f getPosition() const;                  // Get bomb position
    float getExplosionRadius() const;                  // Get explosion radius
    int getDamage() const;                             // Get damage value

private:
    sf::Vector2f m_position;                           // Bomb position
    sf::Sprite m_sprite;                               // Bomb sprite

    bool m_exploded = false;                           // Has bomb exploded
    float m_timer = 0.f;                               // Timer for fuse
    float m_fuseTime = 1.0f;                           // Time before explosion

    float m_explosionRadius = 80.0f;                   // Explosion reach radius
    int m_damage = 150;                                // Damage to robots in range

    std::unique_ptr<AnimationComponent> m_animComponent; // Animation component
    bool m_animationFinished = false;                  // Animation completion flag
};
