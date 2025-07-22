#include "Entities/Projectiles/Bullet.h"
#include "Entities/Base/Robot.h"
#include "Entities/Base/SquadMember.h"
#include "Managers/ResourceManager.h"
#include "Physics/PhysicsUtils.h"
#include "Utils/ConfigLoader.h"
#include <cmath>

Bullet::Bullet(int damage, Robot* target, SquadMember* source)
    : Projectile(GameTypes::ProjectileType::Bullet, damage, 400.0f, target, source)
{
    setupBulletProperties();
}

Bullet::Bullet(int damage, const sf::Vector2f& targetPos, SquadMember* source)
    : Projectile(GameTypes::ProjectileType::Bullet, damage, 400.0f, targetPos, source)
{
    setupBulletProperties();
}

void Bullet::setupBulletProperties() {
    // Load configuration once
    auto& config = ConfigLoader::getInstance();
    float scale = config.getFloat("SquadBullet", "scale", 0.03f);
    std::string textureFile = config.getString("SquadBullet", "texture", "bullet.png");
    m_maxRange = config.getFloat("SquadBullet", "maxRange", 300.0f);

    // Setup sprite
    auto& resourceManager = ResourceManager::getInstance();
    m_sprite.setTexture(resourceManager.getTexture(textureFile));

    sf::FloatRect bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    m_sprite.setScale(scale, scale);
    m_sprite.setPosition(getPosition());

    // Set lifetime
    m_lifetimeTimer.setDuration(3.0f);
    m_lifetimeTimer.restart();
}

void Bullet::update(float dt) {
    Projectile::update(dt);
    m_sprite.setPosition(getPosition());
}

void Bullet::draw(sf::RenderWindow& window) const {
    if (isActive() && !m_shouldRemove) {
        window.draw(m_sprite);
    }
}

void Bullet::createPhysicsBody(b2World* world) {
    createStandardPhysicsBody(world, true, 2.0f, 0.1f, 0.0f);

    if (m_physicsBody) {
        // Set velocity from config
        auto& config = ConfigLoader::getInstance();
        float speed = config.getFloat("SquadBullet", "speed", 400.0f);
        b2Vec2 velocity = PhysicsUtils::sfmlToBox2D(sf::Vector2f(speed, 0.0f));
        m_physicsBody->SetLinearVelocity(velocity);
    }
}

void Bullet::updateTrajectory(float dt) {
    Projectile::updateTrajectory(dt);

    // Check screen bounds
    if (getPosition().x > 1200.0f) {
        m_shouldRemove = true;
        onMiss();
    }
}

void Bullet::onHit() {
    Projectile::onHit();
}

void Bullet::onMiss() {
    Projectile::onMiss();
}

void Bullet::applyEffects(Robot* robot) {
    if (!robot) return;
    robot->takeDamage(m_damage);
    setActive(false);
}

std::unique_ptr<Bullet> Bullet::createStandard(int damage, Robot* target, SquadMember* source) {
    return std::make_unique<Bullet>(damage, target, source);
}