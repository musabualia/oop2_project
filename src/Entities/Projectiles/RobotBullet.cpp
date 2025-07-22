#include "Entities/Projectiles/RobotBullet.h"
#include "Entities/Base/Robot.h"
#include "Entities/Base/SquadMember.h"
#include "Managers/ResourceManager.h"
#include "Managers/AudioManager.h"
#include "Physics/PhysicsUtils.h"
#include "Physics/CollisionCategories.h"
#include "Utils/ConfigLoader.h"

RobotBullet::RobotBullet(int damage, Robot* target, SquadMember* source)
    : Projectile(GameTypes::ProjectileType::RobotBullet, damage, 400.0f, target, source)
{

    setupRobotBulletProperties();
}

RobotBullet::RobotBullet(int damage, const sf::Vector2f& targetPos, SquadMember* source)
    : Projectile(GameTypes::ProjectileType::RobotBullet, damage, 400.0f, targetPos, source)
{
    setupRobotBulletProperties();
}

void RobotBullet::setupRobotBulletProperties() {
    auto& config = ConfigLoader::getInstance();

    m_bulletSpeed = config.getFloat("RobotBullet", "speed", -400.0f);
    m_bulletScale = config.getFloat("RobotBullet", "scale", 0.04f);
    m_maxRange = config.getFloat("RobotBullet", "maxRange", 800.0f);

    // Parse spawn offset
    std::string offsetStr = config.getString("RobotBullet", "spawnOffset", "-30,-50");
    m_spawnOffset = sf::Vector2f(-30.0f, -50.0f);
    size_t comma = offsetStr.find(',');
    if (comma != std::string::npos) {
        try {
            m_spawnOffset.x = std::stof(offsetStr.substr(0, comma));
            m_spawnOffset.y = std::stof(offsetStr.substr(comma + 1));
        }
        catch (const std::exception&) {
            // Use default
        }
    }

    // Load bullet color
    m_bulletColor = sf::Color(255, 0, 0); // Default red

    loadRobotBulletTexture();
    m_lifetimeTimer.setDuration(5.0f);
    m_lifetimeTimer.restart();
}

void RobotBullet::loadRobotBulletTexture() {
    auto& resourceManager = ResourceManager::getInstance();
    std::string textureFile = ConfigLoader::getInstance().getString("RobotBullet", "texture", "robot_bullet.png");
      AudioManager::getInstance().playSound("robot_bullet", AudioManager::AudioCategory::SFX, 0.3f);

    if (resourceManager.hasTexture(textureFile)) {
        m_sprite.setTexture(resourceManager.getTexture(textureFile));
        sf::FloatRect bounds = m_sprite.getLocalBounds();
        m_sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
        m_sprite.setScale(m_bulletScale, m_bulletScale);
    }
    else {
        // Fallback to rectangle shape
        m_bulletShape.setSize(sf::Vector2f(12.0f, 4.0f));
        m_bulletShape.setFillColor(m_bulletColor);
        m_bulletShape.setOrigin(6.0f, 2.0f);
    }
}

void RobotBullet::createPhysicsBody(b2World* world) {
    createStandardPhysicsBody(world, true, 2.0f, 0.05f, 0.0f);

    if (m_physicsBody) {
        m_physicsBody->SetBullet(true);

        // Set collision categories for RobotBullet
        b2Fixture* fixture = m_physicsBody->GetFixtureList();
        if (fixture) {
            b2Filter filter;
            filter.categoryBits = CollisionCategory::ROBOT_BULLET;
            filter.maskBits = CollisionMask::ROBOT_BULLET;
            fixture->SetFilterData(filter);
        }

        // Set velocity
        float velocityMetersPerSec = m_bulletSpeed / PhysicsUtils::PIXELS_PER_METER;
        b2Vec2 velocity(velocityMetersPerSec, 0.0f);
        m_physicsBody->SetLinearVelocity(velocity);
    }
}

void RobotBullet::update(float dt) {
    if (m_hasHit || m_shouldRemove) return;

    m_lifetimeTimer.update(dt);
    if (m_lifetimeTimer.isElapsed()) {
        m_shouldRemove = true;
        onMiss();
        return;
    }

    updatePhysics(dt);
    updateTrajectory(dt);

    if (getPosition().x < -200.0f) {
        m_shouldRemove = true;
        onMiss();
    }
}

void RobotBullet::draw(sf::RenderWindow& window) const {
    if (m_hasHit || m_shouldRemove) return;

    sf::Vector2f currentPos = getPosition();

    if (m_sprite.getTexture()) {
        const_cast<sf::Sprite&>(m_sprite).setPosition(currentPos);
        window.draw(m_sprite);
    }
    else {
        const_cast<sf::RectangleShape&>(m_bulletShape).setPosition(currentPos);
        window.draw(m_bulletShape);
    }
}

void RobotBullet::updateTrajectory(float dt) {
    if (m_hasHit || m_shouldRemove) return;

    if (m_physicsBody) {
        b2Vec2 velocity = m_physicsBody->GetLinearVelocity();
        float speed = std::abs(velocity.x * PhysicsUtils::PIXELS_PER_METER);
        m_travelDistance += speed * dt;
    }

    // Check hit distance
    if (m_hitDistance > 0 && m_travelDistance >= m_hitDistance) {
        onHit();
        return;
    }

    if (m_travelDistance >= m_maxRange) {
        m_shouldRemove = true;
        onMiss();
    }
}

void RobotBullet::onHit() {
    if (m_hasHit) return;

    m_hasHit = true;
    m_shouldRemove = true;

    if (m_physicsBody) {
        m_physicsBody->SetLinearVelocity(b2Vec2(0, 0));
        m_physicsBody->SetEnabled(false);
    }
}

void RobotBullet::onMiss() {
    m_shouldRemove = true;
    if (m_physicsBody) {
        m_physicsBody->SetEnabled(false);
    }
}

void RobotBullet::applyEffects(Robot* target) {
    // Robot bullets don't target robots
}

void RobotBullet::applyEffectsToSquadMember(SquadMember* target) {
    if (!target || m_hasHit) return;

    // Apply damage to squad member (keep manual damage as requested)
    target->onRobotBulletHit(getDamage());
    onHit();
}

bool RobotBullet::canHitSquadMembers() const {
    return true;
}

std::unique_ptr<RobotBullet> RobotBullet::createFromConfig(int damage, const sf::Vector2f& targetPos, Robot* source) {
    return std::make_unique<RobotBullet>(damage, targetPos, nullptr);
}