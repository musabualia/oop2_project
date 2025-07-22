#pragma once
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "Core/Constants.h"

class GameObject {
public:
    virtual ~GameObject();                               // Virtual destructor

    virtual void update(float dt) = 0;                   // Update object logic
    virtual void draw(sf::RenderWindow& window) const = 0;  // Draw object
    virtual GameTypes::ObjectCategory getObjectCategory() const = 0; // Get object category

    virtual void createPhysicsBody(b2World* world) = 0;  // Create physics body
    void destroyPhysicsBody();                           // Destroy physics body
    b2Body* getPhysicsBody() const { return m_physicsBody; } // Get physics body

    sf::Vector2f getPosition() const;                    // Get object position
    void setPosition(const sf::Vector2f& pos);           // Set object position
    void setPosition(float x, float y);                  // Set object position (x, y)

    bool isActive() const;                               // Check if object is active
    void setActive(bool active);                         // Set object active state

    float distanceTo(const GameObject& other) const;     // Calculate distance to another object

protected:
    sf::Sprite m_sprite;                                 // Object's sprite
    sf::Vector2f m_position{ 0.0f, 0.0f };                // Object position
    bool m_active = true;                                // Is object active
    b2Body* m_physicsBody = nullptr;                     // Physics body pointer

    void setupCollisionFilters(b2FixtureDef& fixtureDef) const; // Set collision filters

    void createStandardPhysicsBody(b2World* world, bool isDynamic, float radius,
        float density = 1.0f, float friction = 0.3f);     // Create circular physics body

    void createBoxPhysicsBody(b2World* world, bool isDynamic, float width, float height,
        float density = 1.0f, float friction = 0.3f);     // Create box-shaped physics body

    void updateSpritePosition();                         // Update sprite position to match logic
    void syncSpriteFromPhysics();                        // Sync sprite from physics position
    void syncPhysicsFromSprite();                        // Sync physics from sprite position
};
