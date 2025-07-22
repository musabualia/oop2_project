// ================================
// include/Physics/PhysicsWorld.h
// ================================
#pragma once
#include <box2d/box2d.h>
#include <memory>

class PhysicsContactListener;

class PhysicsWorld {
public:
    PhysicsWorld();
    ~PhysicsWorld();

    void initialize();
    void step(float dt);
    void cleanup();

    b2World* getWorld() { return m_world.get(); }
    void destroyBody(b2Body* body);

private:
    std::unique_ptr<b2World> m_world;
    std::unique_ptr<PhysicsContactListener> m_contactListener;

    float m_timestep;
    int m_velocityIterations;
    int m_positionIterations;
    float m_accumulator = 0.0f;

    static constexpr float DEFAULT_TIMESTEP = 1.0f / 60.0f;
    static constexpr int DEFAULT_VELOCITY_ITERATIONS = 6;
    static constexpr int DEFAULT_POSITION_ITERATIONS = 2;
};