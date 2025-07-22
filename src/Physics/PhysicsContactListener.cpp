// ================================
// src/Physics/PhysicsContactListener.cpp 
// ================================
#include "Physics/PhysicsContactListener.h"
#include "Entities/Base/Robot.h"
#include "Entities/Base/SquadMember.h"
#include "Entities/Base/Projectile.h"
#include "Entities/Base/Collectible.h"
#include "Entities/Projectiles/RobotBullet.h"
#include "Core/GameObject.h"
#include "Core/Constants.h"
#include <iostream>

void PhysicsContactListener::BeginContact(b2Contact* contact) {
    void* userDataA = getUserData(contact->GetFixtureA()->GetBody());
    void* userDataB = getUserData(contact->GetFixtureB()->GetBody());

    if (!userDataA || !userDataB) return;

    if (!isValidGameObject(userDataA) || !isValidGameObject(userDataB)) {
        return;
    }

    GameTypes::ObjectCategory categoryA = getObjectCategory(userDataA);
    GameTypes::ObjectCategory categoryB = getObjectCategory(userDataB);

    // Handle robot-to-robot collisions
    handleRobotRobotCollision(userDataA, userDataB, categoryA, categoryB);

    // Handle robot-squad collisions  
    handleRobotSquadCollision(userDataA, userDataB, categoryA, categoryB);

    if ((categoryA == GameTypes::ObjectCategory::Projectile && categoryB == GameTypes::ObjectCategory::SquadMember) ||
        (categoryA == GameTypes::ObjectCategory::SquadMember && categoryB == GameTypes::ObjectCategory::Projectile)) {

        handleRobotBulletSquadCollision(userDataA, userDataB, categoryA, categoryB);
    }

    // Handle squad projectiles hitting robots
    handleProjectileRobotCollision(userDataA, userDataB, categoryA, categoryB);
}

// Better robot-robot collision handling
void PhysicsContactListener::handleRobotRobotCollision(void* objA, void* objB,
    GameTypes::ObjectCategory catA, GameTypes::ObjectCategory catB) {

    if (catA != GameTypes::ObjectCategory::Robot || catB != GameTypes::ObjectCategory::Robot) {
        return;
    }

    Robot* robotA = static_cast<Robot*>(objA);
    Robot* robotB = static_cast<Robot*>(objB);

    if (!robotA || !robotB || robotA->isDead() || robotB->isDead()) {
        return;
    }

    // Stop both robots temporarily
    if (robotA->getPhysicsBody()) {
        robotA->getPhysicsBody()->SetLinearVelocity(b2Vec2(0, 0));
    }

    if (robotB->getPhysicsBody()) {
        robotB->getPhysicsBody()->SetLinearVelocity(b2Vec2(0, 0));
    }

    // Simple spacing logic - robot that's further right moves back slightly
    float posA = robotA->getPosition().x;
    float posB = robotB->getPosition().x;

    if (posA > posB) {
        // Robot A is further right - move it back slightly
        if (robotA->getPhysicsBody()) {
            b2Vec2 pushBack(0.1f, 0.0f); // Small rightward push
            robotA->getPhysicsBody()->SetLinearVelocity(pushBack);
        }
    }
    else {
        // Robot B is further right - move it back slightly  
        if (robotB->getPhysicsBody()) {
            b2Vec2 pushBack(0.1f, 0.0f); // Small rightward push
            robotB->getPhysicsBody()->SetLinearVelocity(pushBack);
        }
    }
}

void PhysicsContactListener::EndContact(b2Contact* contact) {
    void* userDataA = getUserData(contact->GetFixtureA()->GetBody());
    void* userDataB = getUserData(contact->GetFixtureB()->GetBody());

    if (!userDataA || !userDataB) return;

    if (!isValidGameObject(userDataA) || !isValidGameObject(userDataB)) {
        return;
    }

    GameTypes::ObjectCategory categoryA = getObjectCategory(userDataA);
    GameTypes::ObjectCategory categoryB = getObjectCategory(userDataB);

    // Handle robot-robot collision end
    if (categoryA == GameTypes::ObjectCategory::Robot && categoryB == GameTypes::ObjectCategory::Robot) {
        Robot* robotA = static_cast<Robot*>(userDataA);
        Robot* robotB = static_cast<Robot*>(userDataB);

        if (robotA && robotB && !robotA->isDead() && !robotB->isDead()) {

            // Resume movement for both robots when they separate
            robotA->setMovementEnabled(true);
            robotB->setMovementEnabled(true);
        }
    }
}

void PhysicsContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {}

void* PhysicsContactListener::getUserData(b2Body* body) {
    if (!body) return nullptr;

    uintptr_t userData = body->GetUserData().pointer;
    if (userData == 0) return nullptr;

    return reinterpret_cast<void*>(userData);
}

bool PhysicsContactListener::isValidGameObject(void* userData) {
    if (!userData) return false;

    try {
        auto* gameObject = static_cast<GameObject*>(userData);
        gameObject->isActive();
        return true;
    }
    catch (...) {
        return false;
    }
}

GameTypes::ObjectCategory PhysicsContactListener::getObjectCategory(void* userData) {
    auto* gameObject = static_cast<GameObject*>(userData);
    if (!gameObject) {
        return GameTypes::ObjectCategory::Unknown;
    }

    try {
        return gameObject->getObjectCategory();
    }
    catch (...) {
        return GameTypes::ObjectCategory::Unknown;
    }
}

// ===== ROBOT ↔ SQUAD MEMBER COLLISION =====
void PhysicsContactListener::handleRobotSquadCollision(void* objA, void* objB,
    GameTypes::ObjectCategory catA, GameTypes::ObjectCategory catB) {

    Robot* robot = nullptr;
    SquadMember* squadMember = nullptr;

    if (catA == GameTypes::ObjectCategory::Robot && catB == GameTypes::ObjectCategory::SquadMember) {
        robot = static_cast<Robot*>(objA);
        squadMember = static_cast<SquadMember*>(objB);
    }
    else if (catA == GameTypes::ObjectCategory::SquadMember && catB == GameTypes::ObjectCategory::Robot) {
        robot = static_cast<Robot*>(objB);
        squadMember = static_cast<SquadMember*>(objA);
    }
    else {
        return;
    }

    if (!robot || !squadMember || robot->isDead() || squadMember->isDestroyed()) {
        return;
    }

    robot->onSquadMemberCollision(squadMember);
    squadMember->onRobotCollision(robot);

    int damage = robot->getDamage();
    squadMember->takeDamage(damage);
}

// ===== ROBOT BULLET → SQUAD MEMBER COLLISION =====

void PhysicsContactListener::handleRobotBulletSquadCollision(void* objA, void* objB,
    GameTypes::ObjectCategory catA, GameTypes::ObjectCategory catB) {

    Projectile* projectile = nullptr;
    SquadMember* squadMember = nullptr;

    // Determine which object is which
    if (catA == GameTypes::ObjectCategory::Projectile && catB == GameTypes::ObjectCategory::SquadMember) {
        projectile = static_cast<Projectile*>(objA);
        squadMember = static_cast<SquadMember*>(objB);
    }
    else if (catA == GameTypes::ObjectCategory::SquadMember && catB == GameTypes::ObjectCategory::Projectile) {
        projectile = static_cast<Projectile*>(objB);
        squadMember = static_cast<SquadMember*>(objA);
    }
    else {
        return; // Not the right collision type
    }

    if (!projectile || !squadMember) {
        return;
    }

    if (squadMember->isDestroyed() || !projectile->canHitSquadMembers() || projectile->shouldRemove()) {
        return;
    }

    projectile->applyEffectsToSquadMember(squadMember);

    // Disable physics body to prevent multiple hits
    if (projectile->getPhysicsBody()) {
        projectile->getPhysicsBody()->SetEnabled(false);
    }
}

// ===== PROJECTILE → ROBOT COLLISION (Squad bullets hitting robots) =====
void PhysicsContactListener::handleProjectileRobotCollision(void* objA, void* objB,
    GameTypes::ObjectCategory catA, GameTypes::ObjectCategory catB) {

    Projectile* projectile = nullptr;
    Robot* robot = nullptr;

    if (catA == GameTypes::ObjectCategory::Projectile && catB == GameTypes::ObjectCategory::Robot) {
        projectile = static_cast<Projectile*>(objA);
        robot = static_cast<Robot*>(objB);
    }
    else if (catA == GameTypes::ObjectCategory::Robot && catB == GameTypes::ObjectCategory::Projectile) {
        projectile = static_cast<Projectile*>(objB);
        robot = static_cast<Robot*>(objA);
    }
    else {
        return;
    }

    if (!projectile || !robot || robot->isDead() || projectile->canHitSquadMembers()) {
        return;
    }
    projectile->applyEffects(robot);
}