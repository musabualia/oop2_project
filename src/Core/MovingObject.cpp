// ================================
// Moving Object - Base class for entities that move using Box2D physics
// ================================
#include "Core/MovingObject.h"
#include "Physics/PhysicsUtils.h"

// ================================
// Physics Force Application - Apply force to move object gradually
// ================================
void MovingObject::applyForce(const b2Vec2& force) {
    // Only apply forces to dynamic bodies (not static objects)
    if (m_physicsBody && m_physicsBody->GetType() == b2_dynamicBody) {
        // Apply force to center of mass, wake up body if sleeping
        m_physicsBody->ApplyForceToCenter(force, true);
    }
}

// ================================
// Physics Impulse Application - Apply instant velocity change
// ================================
void MovingObject::applyLinearImpulse(const b2Vec2& impulse) {
    // Impulses cause immediate velocity changes (good for explosions, jumps)
    if (m_physicsBody && m_physicsBody->GetType() == b2_dynamicBody) {
        m_physicsBody->ApplyLinearImpulseToCenter(impulse, true);
    }
}

// ================================
// Direct Velocity Control - Set exact movement speed
// ================================
void MovingObject::setLinearVelocity(const b2Vec2& velocity) {
    // Directly set velocity (used for constant-speed movement like robots)
    if (m_physicsBody && m_physicsBody->GetType() == b2_dynamicBody) {
        m_physicsBody->SetLinearVelocity(velocity);
    }
}

// ================================
// Velocity Query - Get current movement speed and direction
// ================================
b2Vec2 MovingObject::getLinearVelocity() const {
    if (m_physicsBody && m_physicsBody->GetType() == b2_dynamicBody) {
        return m_physicsBody->GetLinearVelocity();
    }
    return b2Vec2(0.0f, 0.0f);  // Return zero velocity if no physics body
}

// ================================
// Physics Synchronization - Keep sprite position synced with physics
// ================================
void MovingObject::updatePhysics(float dt) {
    if (m_physicsBody) {
        // Update sprite position to match physics body position
        // This ensures visual representation matches physics simulation
        syncSpriteFromPhysics();
    }
}