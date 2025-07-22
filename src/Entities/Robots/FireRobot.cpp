#include "Entities/Robots/FireRobot.h"
#include "Entities/Base/SquadMember.h"
#include "Entities/Projectiles/RobotBullet.h"
#include "Managers/SquadMemberManager.h"
#include "Managers/AudioManager.h"
#include "Managers/ProjectileManager.h"
#include "Systems/AnimationSystem.h"
#include "Utils/ConfigLoader.h"
#include <cmath>

FireRobot::FireRobot()
    : Robot(GameTypes::RobotType::Fire, 80, 40.0f, 6)
    , m_deathAnimationStarted(false)
    , m_isShooting(false)
    , m_hasTargetInRange(false)
    , m_shootingTarget(nullptr)
{
    setupFromConfig();
    initializeAnimations();
    setFacingDirection(true);
    m_bulletTimer.setDuration(m_bulletCooldown);
    m_bulletTimer.restart();
}

void FireRobot::setupFromConfig() {
    auto& config = ConfigLoader::getInstance();

    try {
        m_health = config.getInt("FireRobot", "health", 80);
        m_maxHealth = m_health;
        m_rewardValue = config.getInt("FireRobot", "reward", 50);
        m_originalSpeed = config.getFloat("FireRobot", "speed", 40.0f);
        m_attackRange = config.getFloat("FireRobot", "attackRange", 200.0f);
        m_bulletCooldown = config.getFloat("FireRobot", "bulletCooldown", 2.0f);
        m_bulletDamage = config.getInt("RobotBullet", "damage", 35);

        m_damage = 0;
        m_attackDamage = 0;
    }
    catch (const std::exception&) {
        // Use defaults
        m_bulletCooldown = 2.0f;
        m_attackRange = 200.0f;
        m_bulletDamage = 35;
        m_rewardValue = 50;
    }
}

void FireRobot::initializeAnimations() {
    auto& animSystem = AnimationSystem::getInstance();
    animSystem.registerRobot(this);
    switchToWalkingAnimation();
}

void FireRobot::update(float dt) {
    Robot::update(dt);
    if (!isDead()) {
        updateMovementBehavior(dt);
        updateShootingBehavior(dt);
        updateBulletFiring(dt);
    }
}

void FireRobot::updateMovementBehavior(float dt) {
    if (isMovementEnabled() && !isDead() && !m_isShooting) {
        setFacingDirection(true);
    }
}

void FireRobot::updateShootingBehavior(float dt) {
    if (isDead()) {
        if (m_isShooting) stopShooting();
        return;
    }

    if (m_hasTargetInRange && !m_isShooting && m_shootingTarget) {
        startShooting(m_shootingTarget);
    }
    else if (m_isShooting && (!m_hasTargetInRange || !m_shootingTarget || m_shootingTarget->isDestroyed())) {
        stopShooting();
    }
}

void FireRobot::updateBulletFiring(float dt) {
    if (!m_isShooting || !m_shootingTarget || isDead()) return;

    m_bulletTimer.update(dt);
    if (m_bulletTimer.isElapsed() && canFireBullet()) {
        fireBullet();
        m_bulletTimer.restart();
    }
}

bool FireRobot::isPerformingSpecialAction() const {
    return m_isShooting || isAttackingSquadMember() || shouldStopForSquadMember();
}

void FireRobot::updateSquadMemberDetection(float dt) {
    if (!m_squadMemberManager) return;

    sf::Vector2f myPosition = getPosition();
    auto nearbyUnits = m_squadMemberManager->getActiveSquadMembersInRange(myPosition, m_attackRange);

    bool foundTarget = false;
    SquadMember* closestTarget = nullptr;
    float closestDistance = m_attackRange + 1.0f;

    for (SquadMember* unit : nearbyUnits) {
        if (!unit || unit->isDestroyed()) continue;

        if (isSquadMemberInFront(unit)) {
            sf::Vector2f unitPos = unit->getPosition();
            float dist = std::sqrt(std::pow(myPosition.x - unitPos.x, 2) + std::pow(myPosition.y - unitPos.y, 2));

            if (dist < closestDistance) {
                closestDistance = dist;
                closestTarget = unit;
                foundTarget = true;
            }
        }
    }

    if (foundTarget && closestTarget) {
        m_hasTargetInRange = true;
        m_shootingTarget = closestTarget;
        if (!isAttackingSquadMember()) {
            onSquadMemberCollision(closestTarget);
            closestTarget->onRobotCollision(this);
        }
    }
    else {
        m_hasTargetInRange = false;
        m_shootingTarget = nullptr;
    }

    m_hasDetectedTarget = foundTarget;
}

void FireRobot::startShooting(SquadMember* target) {
    if (!target || m_isShooting || isDead()) return;

    m_isShooting = true;
    m_shootingTarget = target;
    setMovementEnabled(false);
    switchToAttackAnimation();
    m_bulletTimer.restart();
}

void FireRobot::stopShooting() {
    if (!m_isShooting) return;

    m_isShooting = false;
    m_shootingTarget = nullptr;
    m_hasTargetInRange = false;

    if (!isDead()) {
        setMovementEnabled(true);
        switchToWalkingAnimation();
    }
}

void FireRobot::fireBullet() {
    if (!canFireBullet()) return;

    sf::Vector2f firePos = getBulletSpawnPosition();
    float hitDistance = -1.0f;

    if (m_shootingTarget && !m_shootingTarget->isDestroyed()) {
        sf::Vector2f robotPos = getPosition();
        sf::Vector2f targetPos = m_shootingTarget->getPosition();
        hitDistance = std::abs(robotPos.x - targetPos.x) - 30.0f;

        m_shootingTarget->takeDamage(35);
    }

    auto& projectileManager = ProjectileManager::getInstance();
    projectileManager.fireRobotBullet(m_bulletDamage, firePos, this);

    if (hitDistance > 0) {
        auto& projectiles = projectileManager.getEntities();
        if (!projectiles.empty()) {
            auto* last = projectiles.back().get();
            if (last->canHitSquadMembers()) {
                static_cast<RobotBullet*>(last)->setHitDistance(hitDistance);
            }
        }
    }
}

bool FireRobot::isReadyToFire() const {
    return m_bulletTimer.isElapsed();
}

bool FireRobot::canFireBullet() const {
    return m_isShooting && m_shootingTarget && !m_shootingTarget->isDestroyed() && !isDead() && m_bulletTimer.isElapsed();
}

sf::Vector2f FireRobot::getBulletSpawnPosition() const {
    sf::Vector2f robotPos = getPosition();
    auto& config = ConfigLoader::getInstance();
    std::string offsetStr = config.getString("RobotBullet", "spawnOffset", "-30,-50");
    sf::Vector2f spawnOffset(-30.0f, -50.0f);

    size_t comma = offsetStr.find(',');
    if (comma != std::string::npos) {
        try {
            spawnOffset.x = std::stof(offsetStr.substr(0, comma));
            spawnOffset.y = std::stof(offsetStr.substr(comma + 1));
        }
        catch (...) {}
    }

    return robotPos + spawnOffset;
}

void FireRobot::updateAnimation(float dt) {
    Robot::updateAnimation(dt);
    auto& animSystem = AnimationSystem::getInstance();
    animSystem.updateEntityPosition(this, getPosition());

    if (!isDead()) {
        setFacingDirection(true);
    }

    if (isDead() && !m_deathAnimationStarted) {
        m_isShooting = false;
        m_shootingTarget = nullptr;
        m_hasTargetInRange = false;
        switchToDeathAnimation();
    }
}

void FireRobot::switchToWalkingAnimation() {
    if (isDead()) return;

    auto& animSystem = AnimationSystem::getInstance();
    m_currentAnimationState = "walking";
    setMovementEnabled(true);
    m_isShooting = false;

    if (animSystem.isEntityAnimationPaused(this)) {
        animSystem.resumeEntityAnimation(this);
    }
    else if (animSystem.hasAnimation("FireRobotWalk")) {
        animSystem.playAnimation(this, "FireRobotWalk");
    }
}

void FireRobot::switchToAttackAnimation() {
    if (isDead()) return;
    AudioManager::getInstance().playSound("robot_bullet");
    auto& animSystem = AnimationSystem::getInstance();
    m_currentAnimationState = "attacking";
    setMovementEnabled(false);
    m_isShooting = true;

    if (animSystem.isEntityPlayingAnimation(this, "FireRobotWalk")) {
        animSystem.pauseEntityAnimation(this);
    }
    else if (animSystem.hasAnimation("FireRobotStatic")) {
        animSystem.playAnimation(this, "FireRobotStatic");
    }
}

void FireRobot::switchToDeathAnimation() {
    if (m_deathAnimationStarted) return;

    m_deathAnimationStarted = true;
    setMovementEnabled(false);
    m_isShooting = false;
    m_shootingTarget = nullptr;
    m_hasTargetInRange = false;
   // AudioManager::getInstance().playSound("stealth_robot_die" ,AudioManager::AudioCategory::SFX, 0.3f);
    auto& animSystem = AnimationSystem::getInstance();
    m_currentAnimationState = "death";

    if (animSystem.isEntityAnimationPaused(this)) {
        animSystem.resumeEntityAnimation(this);
    }

    if (animSystem.hasAnimation("FireRobotDeath")) {
        animSystem.playAnimation(this, "FireRobotDeath");
        animSystem.setAnimationCompleteCallback(this, [this]() {
            setActive(false);
            });
    }
    else {
        setActive(false);
    }
}