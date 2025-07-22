#pragma once
#include "Entities/Base/Projectile.h"
#include "Core/Constants.h"
#include <memory>
#include <string>
#include <unordered_map>

class Robot;
class SquadMember;

class ProjectileFactory {
public:
    static ProjectileFactory& getInstance();                                // Singleton

    void initialize();                                                      // Initialize projectiles

    std::unique_ptr<Projectile> createProjectile(GameTypes::ProjectileType type, int damage, Robot* target, SquadMember* source = nullptr) const; // Create targeting robot
    std::unique_ptr<Projectile> createProjectile(GameTypes::ProjectileType type, int damage, const sf::Vector2f& targetPos, SquadMember* source = nullptr) const; // Create targeting position

    std::unique_ptr<Projectile> createBullet(int damage, Robot* target, SquadMember* source = nullptr) const;         // Standard bullet
    std::unique_ptr<Projectile> createSniperBullet(int damage, Robot* target, SquadMember* source = nullptr) const;   // Sniper bullet
    std::unique_ptr<Projectile> createRobotBullet(int damage, const sf::Vector2f& targetPos) const;                   // Robot bullet

    bool canCreateProjectile(GameTypes::ProjectileType type) const;       // Check if projectile type is enabled

private:
    ProjectileFactory() = default;
    std::unordered_map<GameTypes::ProjectileType, bool> m_enabledTypes;   // Enabled projectile types
};
