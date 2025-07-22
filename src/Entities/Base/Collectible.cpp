#include "Entities/Base/Collectible.h"
#include "Core/Constants.h"

Collectible::Collectible(GameTypes::CollectibleType type, int value, float duration)
    : m_type(type)
    , m_value(value)
    , m_maxLifetime(duration > 0.0f ? duration : GameConstants::COLLECTIBLE_LIFETIME)
{
    m_lifetimeTimer.setDuration(m_maxLifetime);
    m_lifetimeTimer.restart();
}

void Collectible::update(float dt) {
    if (m_collected) return;

    m_lifetimeTimer.update(dt);
    checkExpiration(dt);
    updateSpritePosition();
}

void Collectible::draw(sf::RenderWindow& window) const {
    if (m_collected || !isActive()) return;

    float timeLeft = getRemainingLifetime();
    if (timeLeft < 5.0f && timeLeft > 0.0f) {
        // Could add blinking effect when about to expire
    }
}

GameTypes::CollectibleType Collectible::getType() const {
    return m_type;
}

int Collectible::getValue() const {
    return m_value;
}

bool Collectible::isCollected() const {
    return m_collected;
}

bool Collectible::isExpired() const {
    return m_lifetimeTimer.isElapsed();
}

void Collectible::collect() {
    if (m_collected) return;

    m_collected = true;
    setActive(false);
    applyEffect();
}

void Collectible::spawn(const sf::Vector2f& position) {
    setPosition(position);        
    syncPhysicsFromSprite();        
    m_lifetimeTimer.restart();
    setActive(true);
    m_collected = false;
}


float Collectible::getLifetime() const {
    return m_lifetimeTimer.getElapsed();
}

float Collectible::getRemainingLifetime() const {
    return m_lifetimeTimer.getRemaining();
}

float Collectible::getCollectionRadius() const {
    return m_collectionRadius;
}

void Collectible::setCollectionRadius(float radius) {
    m_collectionRadius = radius;
}

void Collectible::checkExpiration(float dt) {
    if (isExpired() && !m_collected) {
        setActive(false);
    }
}
