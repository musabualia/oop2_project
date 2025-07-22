#include "Entities/Base/Projectile.h"
#include "Entities/Base/Robot.h"
#include "Entities/Base/SquadMember.h"
#include "Physics/PhysicsUtils.h"
#include <cmath>

// Constructor for robot-targeting projectiles
Projectile::Projectile(GameTypes::ProjectileType type, int damage, float speed, Robot* target, SquadMember* source)
    : m_type(type)
    , m_damage(damage)
    , m_targetType(GameTypes::ProjectileTargetType::Robot)
    , m_targetRobot(target)
    , m_source(source)
{
    if (target) {
        m_targetPosition = target->getPosition();
    }

    // Set initial velocity toward target
    if (target) {
        sf::Vector2f direction = target->getPosition() - getPosition();
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (distance > 0.0f) {
            direction /= distance; // Normalize
            // Velocity will be set by physics body
        }
    }

    // Initialize lifetime timer
    m_lifetimeTimer.setDuration(GameConstants::PROJECTILE_LIFETIME);
    m_lifetimeTimer.restart();
}

// Constructor for position-targeting projectiles
Projectile::Projectile(GameTypes::ProjectileType type, int damage, float speed, const sf::Vector2f& targetPos, SquadMember* source)
    : m_type(type)
    , m_damage(damage)
    , m_targetType(GameTypes::ProjectileTargetType::Position)
    , m_targetPosition(targetPos)
    , m_source(source)
{
    m_lifetimeTimer.setDuration(GameConstants::PROJECTILE_LIFETIME);
    m_lifetimeTimer.restart();
}

void Projectile::update(float dt) {
    if (m_shouldRemove) return;

    // Update target position if tracking robot
    if (m_targetType == GameTypes::ProjectileTargetType::Robot && m_targetRobot) {
        if (m_targetRobot->isDead() || !m_targetRobot->isActive()) {
            m_shouldRemove = true;
            onMiss();
            return;
        }
        m_targetPosition = m_targetRobot->getPosition();
    }

    updatePhysics(dt);
    updateTrajectory(dt);
    checkLifetime(dt);

    if (!m_shouldRemove && hasHitTarget()) {
        onHit();
    }
}

void Projectile::draw(sf::RenderWindow& window) const {
    // Default: empty - derived classes handle their own rendering
}

void Projectile::createPhysicsBody(b2World* world) {
    // Create a small circular physics body for the projectile
    createStandardPhysicsBody(world, true, 2.0f, 0.1f, 0.0f);
}

GameTypes::ProjectileType Projectile::getType() const {
    return m_type;
}

int Projectile::getDamage() const {
    return m_damage;
}

bool Projectile::shouldRemove() const {
    return m_shouldRemove || !m_active || isExpired();
}

Robot* Projectile::getTargetRobot() const {
    return m_targetRobot;
}

sf::Vector2f Projectile::getTargetPosition() const {
    return m_targetPosition;
}

SquadMember* Projectile::getSource() const {
    return m_source;
}

GameTypes::ProjectileTargetType Projectile::getTargetType() const {
    return m_targetType;
}

bool Projectile::hasHitTarget() const {
    if (m_hasHit) return true;

    float hitRadius = 10.0f;

    if (m_targetType == GameTypes::ProjectileTargetType::Robot && m_targetRobot) {
        if (!isTargetValid()) return false;
        return distanceToTarget() <= hitRadius;
    }
    else if (m_targetType == GameTypes::ProjectileTargetType::Position) {
        return distanceToTarget() <= hitRadius;
    }

    return false;
}

void Projectile::onHit() {
    if (m_hasHit) return;

    m_hasHit = true;
    m_shouldRemove = true;

    // Apply effects to target if it's a robot
    if (m_targetType == GameTypes::ProjectileTargetType::Robot && m_targetRobot) {
        applyEffects(m_targetRobot);
    }
}

void Projectile::onMiss() {
    m_shouldRemove = true;
}

void Projectile::updateTrajectory(float dt) {
    // Basic trajectory: let physics handle movement
    if (m_physicsBody) {
        b2Vec2 velocity = m_physicsBody->GetLinearVelocity();
        float speed = velocity.Length() * PhysicsUtils::PIXELS_PER_METER;
        m_travelDistance += speed * dt;
    }

    // Check max range
    if (m_travelDistance >= m_maxRange) {
        onMiss();
    }
}

void Projectile::applyEffects(Robot* target) {
    // Default: just apply damage
    if (target && !m_hasHit) {
        target->takeDamage(m_damage);
    }
}

void Projectile::applyEffectsToSquadMember(SquadMember* target) {
    if (target && !m_hasHit && canHitSquadMembers()) {
        target->onRobotBulletHit(m_damage);
        m_hasHit = true;
        m_shouldRemove = true;
    }
}

bool Projectile::isExpired() const {
    return m_lifetimeTimer.isElapsed();
}

float Projectile::getLifetime() const {
    return m_lifetimeTimer.getElapsed();
}

float Projectile::getRemainingLifetime() const {
    return m_lifetimeTimer.getRemaining();
}

void Projectile::checkLifetime(float dt) {
    m_lifetimeTimer.update(dt);
    if (m_lifetimeTimer.isElapsed()) {
        onMiss();
    }
}

bool Projectile::isTargetValid() const {
    if (m_targetType == GameTypes::ProjectileTargetType::Robot && m_targetRobot) {
        return !m_targetRobot->isDead() && m_targetRobot->isActive();
    }
    return true;
}

float Projectile::distanceToTarget() const {
    sf::Vector2f currentPos = getPosition();
    sf::Vector2f diff = currentPos - m_targetPosition;
    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}