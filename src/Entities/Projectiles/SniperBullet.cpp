#include "Entities/Projectiles/SniperBullet.h"
#include "Entities/Base/Robot.h"
#include "Entities/Base/SquadMember.h"
#include "Managers/ResourceManager.h"
#include "Physics/PhysicsUtils.h"
#include "Utils/ConfigLoader.h"

SniperBullet::SniperBullet(int damage, Robot* target, SquadMember* source)
    : Projectile(GameTypes::ProjectileType::SniperBullet, damage, 600.0f, target, source)
{
    setupSniperBulletProperties();
}

SniperBullet::SniperBullet(int damage, const sf::Vector2f& targetPos, SquadMember* source)
    : Projectile(GameTypes::ProjectileType::SniperBullet, damage, 600.0f, targetPos, source)
{
    setupSniperBulletProperties();
}

void SniperBullet::setupSniperBulletProperties() {
    // Load configuration once
    auto& config = ConfigLoader::getInstance();
    float scale = config.getFloat("SniperBullet", "scale", 0.06f);
    std::string textureFile = config.getString("SniperBullet", "texture", "SniperBullet.png");
    m_maxRange = config.getFloat("SniperBullet", "maxRange", 350.0f);

    // Setup sprite
    auto& resourceManager = ResourceManager::getInstance();
    m_sprite.setTexture(resourceManager.getTexture(textureFile));

    sf::FloatRect bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    m_sprite.setScale(scale, scale);
    m_sprite.setPosition(getPosition());

    m_lifetimeTimer.setDuration(3.0f);
    m_lifetimeTimer.restart();
}

void SniperBullet::update(float dt) {
    Projectile::update(dt);
    m_sprite.setPosition(getPosition());
}

void SniperBullet::draw(sf::RenderWindow& window) const {
    if (isActive() && !m_shouldRemove) {
        window.draw(m_sprite);
    }
}

void SniperBullet::createPhysicsBody(b2World* world) {
    createStandardPhysicsBody(world, true, 2.0f, 0.1f, 0.0f);

    if (m_physicsBody) {
        auto& config = ConfigLoader::getInstance();
        float speed = config.getFloat("SniperBullet", "speed", 600.0f);
        b2Vec2 velocity = PhysicsUtils::sfmlToBox2D(sf::Vector2f(speed, 0.0f));
        m_physicsBody->SetLinearVelocity(velocity);
    }
}

void SniperBullet::updateTrajectory(float dt) {
    Projectile::updateTrajectory(dt);

    if (getPosition().x > 1200.0f) {
        m_shouldRemove = true;
        onMiss();
    }
}

void SniperBullet::onHit() {
    Projectile::onHit();
}

void SniperBullet::onMiss() {
    Projectile::onMiss();
}

void SniperBullet::applyEffects(Robot* robot) {
    if (!robot) return;
    robot->takeDamage(m_damage);
    setActive(false);
}

std::unique_ptr<SniperBullet> SniperBullet::createStandard(int damage, Robot* target, SquadMember* source) {
    return std::make_unique<SniperBullet>(damage, target, source);
}