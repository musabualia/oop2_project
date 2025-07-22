#pragma once
#include "Managers/EntityManager.h"
#include "Entities/Base/Projectile.h"
#include "Core/Constants.h"
#include "Core/Timer.h"
#include <SFML/Graphics.hpp>
#include <memory>

// Forward declarations
class Robot;
class SquadMember;
class PhysicsWorld;

class ProjectileManager : public EntityManager<Projectile> {
public:
    static ProjectileManager& getInstance();
    ProjectileManager(const ProjectileManager&) = delete;
    ProjectileManager& operator=(const ProjectileManager&) = delete;

    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;

    void fireProjectile(GameTypes::ProjectileType type, int damage, Robot* target, SquadMember* source = nullptr);
    void fireProjectile(GameTypes::ProjectileType type, int damage, const sf::Vector2f& targetPos, SquadMember* source = nullptr);
    void fireProjectileFromUnit(SquadMember* source, Robot* target);
    void fireRobotBullet(int damage, const sf::Vector2f& firePosition, Robot* source = nullptr);
    
    void setPhysicsWorld(PhysicsWorld* physicsWorld);
   
    int getProjectileCount() const;

private:
    // Private constructor for singleton
    ProjectileManager();
    ~ProjectileManager() = default;

    PhysicsWorld* m_physicsWorld = nullptr; // Associated physics world
    Timer m_cleanupTimer;                   // Timer for cleanup checks
    int m_maxProjectiles = 1000;            // Max allowed active projectiles

    int getProjectileDamageFromConfig(GameTypes::ProjectileType type) const;
    GameTypes::ProjectileType getProjectileTypeForSquadMember(GameTypes::SquadMemberType squadType) const;
    sf::Vector2f getSpawnOffsetForProjectile(GameTypes::ProjectileType type) const;
    void cleanupExpiredProjectiles();
};
