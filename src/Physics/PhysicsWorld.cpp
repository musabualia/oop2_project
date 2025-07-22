#include "Physics/PhysicsWorld.h"
#include "Physics/PhysicsContactListener.h"
#include "Core/Constants.h"

PhysicsWorld::PhysicsWorld()
    : m_timestep(DEFAULT_TIMESTEP)
    , m_velocityIterations(DEFAULT_VELOCITY_ITERATIONS)
    , m_positionIterations(DEFAULT_POSITION_ITERATIONS) {

}

PhysicsWorld::~PhysicsWorld() {
    cleanup();
}

void PhysicsWorld::initialize() {
    // Create Box2D world with gravity
    b2Vec2 gravity(0.0f, 0.0f);
    m_world = std::make_unique<b2World>(gravity);

    // Set up contact listener
    m_contactListener = std::make_unique<PhysicsContactListener>();
    m_world->SetContactListener(m_contactListener.get());
}

void PhysicsWorld::step(float dt) {
    if (!m_world) return;

    m_accumulator += dt;

    while (m_accumulator >= m_timestep) {
        m_world->Step(m_timestep, m_velocityIterations, m_positionIterations);
        m_accumulator -= m_timestep;
    }
}

void PhysicsWorld::cleanup() {
    if (m_world) {
        b2Body* body = m_world->GetBodyList();
        while (body) {
            b2Body* next = body->GetNext();
            // Clear user data to prevent collision callbacks
            body->GetUserData().pointer = 0;
            m_world->DestroyBody(body);
            body = next;
        }

        // Now safe to destroy the world
        m_world.reset();
    }
    m_contactListener.reset();
}

void PhysicsWorld::destroyBody(b2Body* body) {
    if (m_world && body) {
        m_world->DestroyBody(body);
    }
}