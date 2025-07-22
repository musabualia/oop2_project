// ================================
// Game Object - Base class for all game entities with physics integration
// ================================
#include "Core/GameObject.h"
#include "Physics/PhysicsUtils.h"
#include "Systems/AnimationSystem.h" 
#include "Physics/CollisionCategories.h"
#include <cmath>

// ================================
// Destructor - Clean up physics and animation resources
// ================================
GameObject::~GameObject() {
    // Unregister from animation system before destruction
    AnimationSystem::getInstance().unregisterEntity(this);
    destroyPhysicsBody();
}

// ================================
// Physics Body Cleanup - Safely destroy Box2D physics body
// ================================
void GameObject::destroyPhysicsBody() {
    if (m_physicsBody) {
        b2World* world = nullptr;
        try {
            world = m_physicsBody->GetWorld();
        }
        catch (...) {
            // Handle case where world is already destroyed
            m_physicsBody = nullptr;
            return;
        }

        if (world) {
            // Clear user data to prevent dangling pointer access
            m_physicsBody->GetUserData().pointer = 0;
            world->DestroyBody(m_physicsBody);
        }

        m_physicsBody = nullptr;
    }
}

// ================================
// Position Management - Handle position with physics integration
// ================================
sf::Vector2f GameObject::getPosition() const {
    // If physics body exists, use physics position (authoritative)
    if (m_physicsBody) {
        b2Vec2 pos = m_physicsBody->GetPosition();
        return PhysicsUtils::box2DToSFML(pos);
    }
    // Otherwise use stored position
    return m_position;
}

void GameObject::setPosition(const sf::Vector2f& pos) {
    m_position = pos;

    // Update physics body position if it exists
    if (m_physicsBody) {
        m_physicsBody->SetTransform(PhysicsUtils::sfmlToBox2D(pos), m_physicsBody->GetAngle());
    }

    updateSpritePosition();
}

void GameObject::setPosition(float x, float y) {
    setPosition(sf::Vector2f(x, y));
}

bool GameObject::isActive() const {
    return m_active;
}

void GameObject::setActive(bool active) {
    m_active = active;
}

// Keep essential utility method (used by Projectiles/Robots)
float GameObject::distanceTo(const GameObject& other) const {
    sf::Vector2f diff = getPosition() - other.getPosition();
    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}

void GameObject::updateSpritePosition() {
    m_sprite.setPosition(m_position);
}

void GameObject::syncSpriteFromPhysics() {
    if (m_physicsBody) {
        b2Vec2 pos = m_physicsBody->GetPosition();
        m_position = PhysicsUtils::box2DToSFML(pos);
        updateSpritePosition();
    }
}

void GameObject::syncPhysicsFromSprite() {
    if (m_physicsBody) {
        m_physicsBody->SetTransform(PhysicsUtils::sfmlToBox2D(m_position), m_physicsBody->GetAngle());
    }
}

// Physics helper methods
void GameObject::setupCollisionFilters(b2FixtureDef& fixtureDef) const {
    GameTypes::ObjectCategory category = getObjectCategory();
    switch (category) {
    case GameTypes::ObjectCategory::Robot:
        fixtureDef.filter.categoryBits = CollisionCategory::ROBOT;
        fixtureDef.filter.maskBits = CollisionMask::ROBOT;
        break;
    case GameTypes::ObjectCategory::SquadMember:
        fixtureDef.filter.categoryBits = CollisionCategory::SQUAD_MEMBER;
        fixtureDef.filter.maskBits = CollisionMask::SQUAD_MEMBER;
        break;
    case GameTypes::ObjectCategory::Projectile:
        fixtureDef.filter.categoryBits = CollisionCategory::PROJECTILE;
        fixtureDef.filter.maskBits = CollisionMask::PROJECTILE;
        break;
    case GameTypes::ObjectCategory::Collectible:
        fixtureDef.filter.categoryBits = CollisionCategory::COLLECTIBLE;
        fixtureDef.filter.maskBits = CollisionMask::COLLECTIBLE;
        break;
    default:
        fixtureDef.filter.categoryBits = 0xFFFF;
        fixtureDef.filter.maskBits = 0xFFFF;
        break;
    }
}

void GameObject::createStandardPhysicsBody(b2World* world, bool isDynamic, float radius,
    float density, float friction) {
    if (!world || m_physicsBody) return;

    b2BodyDef bodyDef;
    bodyDef.type = isDynamic ? b2_dynamicBody : b2_staticBody;
    bodyDef.position = PhysicsUtils::sfmlToBox2D(getPosition());
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);

    m_physicsBody = world->CreateBody(&bodyDef);

    b2CircleShape circleShape;
    circleShape.m_radius = PhysicsUtils::sfmlToBox2D(radius);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circleShape;
    fixtureDef.density = density;
    fixtureDef.friction = friction;

    setupCollisionFilters(fixtureDef);
    m_physicsBody->CreateFixture(&fixtureDef);
}

void GameObject::createBoxPhysicsBody(b2World* world, bool isDynamic, float width, float height,
    float density, float friction) {
    if (!world || m_physicsBody) return;

    b2BodyDef bodyDef;
    bodyDef.type = isDynamic ? b2_dynamicBody : b2_staticBody;
    bodyDef.position = PhysicsUtils::sfmlToBox2D(getPosition());
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);

    m_physicsBody = world->CreateBody(&bodyDef);

    b2PolygonShape boxShape;
    boxShape.SetAsBox(PhysicsUtils::sfmlToBox2D(width / 2.0f), PhysicsUtils::sfmlToBox2D(height / 2.0f));

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.density = density;
    fixtureDef.friction = friction;

    setupCollisionFilters(fixtureDef);
    m_physicsBody->CreateFixture(&fixtureDef);
}