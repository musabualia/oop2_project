#include "Systems/AnimationSystem.h"
#include "Entities/Collectibles/HealthPack.h"
#include "Physics/PhysicsUtils.h"
#include "Managers/AudioManager.h"

HealthPack::HealthPack(int healAmount)
    : Collectible(GameTypes::CollectibleType::HealthPack, healAmount),
    m_healAmount(healAmount)
{
    auto& animSystem = AnimationSystem::getInstance();
    AudioManager::getInstance().playSound("health_kit_added");
    animSystem.registerCollectible(this);

    if (animSystem.hasAnimation("HealthBag")) {
        animSystem.playAnimation(this, "HealthBag");
    }
}

void HealthPack::update(float dt) {
    AnimationSystem::getInstance().updateEntityPosition(this, getPosition());
    Collectible::update(dt);
}

void HealthPack::draw(sf::RenderWindow& window) const {
    if (!isActive() || isCollected()) return;
}

void HealthPack::applyEffect() {
    // Effect logic goes here
}

void HealthPack::healSquadMember(SquadMember* member) {
    if (canHealSquadMember(member)) {
        // Healing logic to be implemented in derived classes or future
    }
}

bool HealthPack::canHealSquadMember(const SquadMember* member) const {
    return member != nullptr;
}

int HealthPack::getHealAmount() const {
    return m_healAmount;
}

void HealthPack::createPhysicsBody(b2World* world) {
    if (!world) return;

    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position = PhysicsUtils::sfmlToBox2D(getPosition());
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);

    m_physicsBody = world->CreateBody(&bodyDef);
    if (!m_physicsBody) return;

    b2PolygonShape boxShape;
    float halfSize = PhysicsUtils::sfmlToBox2D(12.0f);
    boxShape.SetAsBox(halfSize, halfSize);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.isSensor = true;

    m_physicsBody->CreateFixture(&fixtureDef);
}
