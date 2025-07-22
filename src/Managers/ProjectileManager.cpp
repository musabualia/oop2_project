// ================================
// Projectile Manager - Singleton Entity Manager for Projectile Lifecycle and Physics
// ================================
#include "Managers/ProjectileManager.h"
#include "Factories/ProjectileFactory.h"
#include "Entities/Base/Projectile.h"
#include "Entities/Base/Robot.h"
#include "Entities/Base/SquadMember.h"
#include "Physics/PhysicsWorld.h"
#include "Utils/ConfigLoader.h"
#include <iostream>

// Singleton access point
ProjectileManager& ProjectileManager::getInstance() {
    static ProjectileManager instance;
    return instance;
}

// Private constructor - initialize factory and cleanup timer
ProjectileManager::ProjectileManager() {
    m_cleanupTimer.setDuration(1.0f);
    m_cleanupTimer.restart();
    ProjectileFactory::getInstance().initialize();
}

// Main update loop - manages projectile lifecycle and cleanup
void ProjectileManager::update(float dt) {
    // Update all active projectiles
    for (auto& projectile : m_entities) {
        if (projectile && projectile->isActive()) {
            projectile->update(dt);
        }
    }

    // Remove inactive projectiles from entity list
    removeInactive();

    // Periodic cleanup to maintain performance
    m_cleanupTimer.update(dt);
    if (m_cleanupTimer.isElapsed()) {
        cleanupExpiredProjectiles();
        m_cleanupTimer.restart();
    }
}

// Render all active projectiles
void ProjectileManager::draw(sf::RenderWindow& window) const {
    for (const auto& projectile : m_entities) {
        if (projectile && projectile->isActive()) {
            projectile->draw(window);
        }
    }
}

// Fire projectile at specific robot target
void ProjectileManager::fireProjectile(GameTypes::ProjectileType type, int damage, Robot* target, SquadMember* source) {
    if (!target || !source || static_cast<int>(m_entities.size()) >= m_maxProjectiles) return;

    // Get damage from configuration file
    int configDamage = getProjectileDamageFromConfig(type);

    // Create projectile using Factory Pattern
    auto& factory = ProjectileFactory::getInstance();
    auto projectile = factory.createProjectile(type, configDamage, target, source);

    if (projectile) {
        // Position projectile at source with spawn offset
        sf::Vector2f sourcePos = source->getPosition();
        sf::Vector2f spawnOffset = getSpawnOffsetForProjectile(type);
        projectile->setPosition(sourcePos + spawnOffset);

        // Create physics body for collision detection
        if (m_physicsWorld) {
            projectile->createPhysicsBody(m_physicsWorld->getWorld());
        }

        // Add to entity manager
        addEntity(std::move(projectile));
    }
}

// Fire projectile at specific position
void ProjectileManager::fireProjectile(GameTypes::ProjectileType type, int damage, const sf::Vector2f& targetPos, SquadMember* source) {
    if (static_cast<int>(m_entities.size()) >= m_maxProjectiles) return;

    // Get damage from configuration
    int configDamage = getProjectileDamageFromConfig(type);

    // Create projectile using Factory Pattern
    auto& factory = ProjectileFactory::getInstance();
    auto projectile = factory.createProjectile(type, configDamage, targetPos, source);

    if (projectile) {
        // Position projectile
        if (source) {
            sf::Vector2f sourcePos = source->getPosition();
            sf::Vector2f spawnOffset = getSpawnOffsetForProjectile(type);
            projectile->setPosition(sourcePos + spawnOffset);
        }
        else {
            sf::Vector2f spawnOffset = getSpawnOffsetForProjectile(type);
            projectile->setPosition(targetPos + spawnOffset);
        }

        // Create physics body for collision detection
        if (m_physicsWorld) {
            projectile->createPhysicsBody(m_physicsWorld->getWorld());
        }

        // Add to entity manager
        addEntity(std::move(projectile));
    }
}

// Fire projectile from squad member to robot (automatically determines type)
void ProjectileManager::fireProjectileFromUnit(SquadMember* source, Robot* target) {
    if (!source || !target) return;

    // Determine projectile type based on squad member type
    GameTypes::ProjectileType type = getProjectileTypeForSquadMember(source->getType());
    fireProjectile(type, 0, target, source);  // Damage will be loaded from config
}

// Fire robot bullet (enemy projectile)
void ProjectileManager::fireRobotBullet(int damage, const sf::Vector2f& firePosition, Robot* source) {
    if (static_cast<int>(m_entities.size()) >= m_maxProjectiles) {
        return;
    }

    // Get damage from configuration
    int configDamage = getProjectileDamageFromConfig(GameTypes::ProjectileType::RobotBullet);

    // Create robot bullet using Factory Pattern
    auto& factory = ProjectileFactory::getInstance();
    auto robotBullet = factory.createRobotBullet(configDamage, firePosition);

    if (robotBullet) {
        robotBullet->setPosition(firePosition);

        // Create physics body for collision detection
        if (m_physicsWorld) {
            robotBullet->createPhysicsBody(m_physicsWorld->getWorld());
        }

        // Add to entity manager
        addEntity(std::move(robotBullet));
    }
}

// Set physics world for collision detection
void ProjectileManager::setPhysicsWorld(PhysicsWorld* physicsWorld) {
    m_physicsWorld = physicsWorld;
}

// Get current projectile count for performance monitoring
int ProjectileManager::getProjectileCount() const {
    return static_cast<int>(activeSize());
}

// Private Helper Methods

// Get projectile damage from configuration files
int ProjectileManager::getProjectileDamageFromConfig(GameTypes::ProjectileType type) const {
    auto& config = ConfigLoader::getInstance();

    switch (type) {
    case GameTypes::ProjectileType::Bullet:
        return config.getInt("SquadBullet", "damage", 30);
    case GameTypes::ProjectileType::SniperBullet:
        return config.getInt("SniperBullet", "damage", 40);
    case GameTypes::ProjectileType::RobotBullet:
        return config.getInt("RobotBullet", "damage", 35);
    default:
        return 25;  // Default damage fallback
    }
}

// Determine projectile type based on squad member type
GameTypes::ProjectileType ProjectileManager::getProjectileTypeForSquadMember(GameTypes::SquadMemberType squadType) const {
    switch (squadType) {
    case GameTypes::SquadMemberType::HeavyGunner:
        return GameTypes::ProjectileType::Bullet;
    case GameTypes::SquadMemberType::Sniper:
        return GameTypes::ProjectileType::SniperBullet;
    case GameTypes::SquadMemberType::ShieldBearer:
        return GameTypes::ProjectileType::Bullet;  // Shield bearers don't shoot, but fallback
    default:
        return GameTypes::ProjectileType::Bullet;
    }
}

// Get spawn offset for projectile based on type (from config)
sf::Vector2f ProjectileManager::getSpawnOffsetForProjectile(GameTypes::ProjectileType type) const {
    auto& config = ConfigLoader::getInstance();
    std::string sectionName;

    // Determine config section based on projectile type
    switch (type) {
    case GameTypes::ProjectileType::Bullet:
        sectionName = "SquadBullet";
        break;
    case GameTypes::ProjectileType::RobotBullet:
        sectionName = "RobotBullet";
        break;
    case GameTypes::ProjectileType::SniperBullet:
        sectionName = "SniperBullet";
        break;
    default:
        return sf::Vector2f(25.0f, 0.0f);  // Default offset
    }

    // Parse spawn offset from config string (format: "x,y")
    std::string offsetStr = config.getString(sectionName, "spawnOffset", "25,0");
    sf::Vector2f spawnOffset(25.0f, 0.0f);

    size_t comma = offsetStr.find(',');
    if (comma != std::string::npos) {
        try {
            spawnOffset.x = std::stof(offsetStr.substr(0, comma));
            spawnOffset.y = std::stof(offsetStr.substr(comma + 1));
        }
        catch (const std::exception&) {
            // Use default offset on parse error
        }
    }

    return spawnOffset;
}

// Cleanup expired projectiles for memory management
void ProjectileManager::cleanupExpiredProjectiles() {
    auto it = std::remove_if(m_entities.begin(), m_entities.end(),
        [](const std::unique_ptr<Projectile>& p) {
            return !p || p->shouldRemove() || !p->isActive();
        });

    if (it != m_entities.end()) {
        m_entities.erase(it, m_entities.end());
    }
}