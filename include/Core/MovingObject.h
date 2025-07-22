#pragma once
#include "GameObject.h"
#include <box2d/box2d.h>

class MovingObject : public GameObject {
public:
    virtual ~MovingObject() = default;                      // Default destructor

    void createPhysicsBody(b2World* world) override {       // Create default circular dynamic body
        createStandardPhysicsBody(world, true, 15.0f, 1.0f, 0.3f);
    }

    virtual void applyForce(const b2Vec2& force);           // Apply force to the object
    virtual void applyLinearImpulse(const b2Vec2& impulse); // Apply linear impulse
    virtual void setLinearVelocity(const b2Vec2& velocity); // Set linear velocity
    virtual b2Vec2 getLinearVelocity() const;               // Get current linear velocity

protected:
    virtual void updatePhysics(float dt);                   // Update physics state
};
