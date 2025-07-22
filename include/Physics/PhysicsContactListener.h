#pragma once
#include <box2d/box2d.h>
#include "Core/Constants.h"

class PhysicsContactListener : public b2ContactListener {
public:
    void BeginContact(b2Contact* contact) override;
    void handleRobotRobotCollision(void* objA, void* objB, GameTypes::ObjectCategory catA, GameTypes::ObjectCategory catB);
    void EndContact(b2Contact* contact) override;
    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;

private:

    void* getUserData(b2Body* body);
    bool isValidGameObject(void* userData);
    GameTypes::ObjectCategory getObjectCategory(void* userData);
    void handleRobotSquadCollision(void* objA, void* objB, GameTypes::ObjectCategory catA, GameTypes::ObjectCategory catB);
    void handleRobotBulletSquadCollision(void* objA, void* objB, GameTypes::ObjectCategory catA, GameTypes::ObjectCategory catB);
    void handleProjectileRobotCollision(void* objA, void* objB, GameTypes::ObjectCategory catA, GameTypes::ObjectCategory catB);
    void handleCollectiblePickup(void* objA, void* objB, GameTypes::ObjectCategory catA, GameTypes::ObjectCategory catB);
};
