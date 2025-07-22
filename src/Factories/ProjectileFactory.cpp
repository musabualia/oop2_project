#include "Factories/ProjectileFactory.h"
#include "Entities/Projectiles/Bullet.h"
#include "Entities/Projectiles/SniperBullet.h"
#include "Entities/Projectiles/RobotBullet.h"
#include "Entities/Base/Robot.h"
#include "Entities/Base/SquadMember.h"
#include "Utils/ConfigLoader.h"

ProjectileFactory& ProjectileFactory::getInstance() {
    static ProjectileFactory instance;
    return instance;
}

void ProjectileFactory::initialize() {
    // Load projectiles config file
    auto& config = ConfigLoader::getInstance();
    config.loadFromFile("projectiles.cfg");

    // Enable available projectile types
    m_enabledTypes[GameTypes::ProjectileType::Bullet] = true;
    m_enabledTypes[GameTypes::ProjectileType::RobotBullet] = true;
    m_enabledTypes[GameTypes::ProjectileType::SniperBullet] = true;
}

std::unique_ptr<Projectile> ProjectileFactory::createProjectile(GameTypes::ProjectileType type, int damage, Robot* target, SquadMember* source) const {
    if (!canCreateProjectile(type)) {
        return nullptr;
    }

    switch (type) {
    case GameTypes::ProjectileType::Bullet:
        return createBullet(damage, target, source);

    case GameTypes::ProjectileType::SniperBullet:
        return createSniperBullet(damage, target, source);

    case GameTypes::ProjectileType::RobotBullet:
        // Robot bullets target positions, not robots
        return nullptr;

    default:
        return nullptr;
    }
}

std::unique_ptr<Projectile> ProjectileFactory::createProjectile(GameTypes::ProjectileType type, int damage, const sf::Vector2f& targetPos, SquadMember* source) const {
    if (!canCreateProjectile(type)) {
        return nullptr;
    }

    switch (type) {
    case GameTypes::ProjectileType::Bullet:
        return std::make_unique<Bullet>(damage, targetPos, source);

    case GameTypes::ProjectileType::SniperBullet:
        return std::make_unique<SniperBullet>(damage, targetPos, source);

    case GameTypes::ProjectileType::RobotBullet:
        return createRobotBullet(damage, targetPos);

    default:
        return nullptr;
    }
}

std::unique_ptr<Projectile> ProjectileFactory::createBullet(int damage, Robot* target, SquadMember* source) const {
    return std::make_unique<Bullet>(damage, target, source);
}

std::unique_ptr<Projectile> ProjectileFactory::createSniperBullet(int damage, Robot* target, SquadMember* source) const {
    return std::make_unique<SniperBullet>(damage, target, source);
}

std::unique_ptr<Projectile> ProjectileFactory::createRobotBullet(int damage, const sf::Vector2f& targetPos) const {
    return RobotBullet::createFromConfig(damage, targetPos, nullptr);
}

bool ProjectileFactory::canCreateProjectile(GameTypes::ProjectileType type) const {
    auto it = m_enabledTypes.find(type);
    return (it != m_enabledTypes.end()) ? it->second : false;
}