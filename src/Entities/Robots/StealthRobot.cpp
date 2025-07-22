#include "Entities/Robots/StealthRobot.h"
#include "Entities/Base/SquadMember.h"
#include "Managers/SquadMemberManager.h"
#include "Managers/AudioManager.h"
#include "Systems/AnimationSystem.h"
#include "Utils/ConfigLoader.h"
#include "Core/Constants.h"
#include <cmath>

StealthRobot::StealthRobot()
    : Robot(GameTypes::RobotType::Stealth, 70, 60.0f, 25)  // These will be overridden by config
    , m_deathAnimationStarted(false)
    , m_isAttacking(false)
{
    setupFromConfig();
    initializeAnimations();
    setFacingDirection(true);
}

void StealthRobot::setupFromConfig() {
    auto& config = ConfigLoader::getInstance();

    try {
        m_health = config.getInt("StealthRobot", "health", 70);
        m_maxHealth = m_health;
        m_damage = config.getInt("StealthRobot", "damage", 40);
        m_rewardValue = config.getInt("StealthRobot", "reward", 35);
        m_originalSpeed = config.getFloat("StealthRobot", "speed", 60.0f);
        m_attackRange = config.getFloat("StealthRobot", "attackRange", 100.0f);

        m_attackDamage = m_damage;
    }
    catch (const std::exception& e) {
        // Constructor defaults will be used
    }
}

void StealthRobot::initializeAnimations() {
    auto& animSystem = AnimationSystem::getInstance();
    animSystem.registerRobot(this);
    switchToWalkingAnimation();
}

void StealthRobot::update(float dt) {
    Robot::update(dt);
    if (!isDead()) {
        updateMovementBehavior(dt);
    }
}

void StealthRobot::updateMovementBehavior(float dt) {
    if (isMovementEnabled() && !isDead() && !m_isAttacking) {
        setFacingDirection(true);
    }
}

bool StealthRobot::isPerformingSpecialAction() const {
    return isAttackingSquadMember() || shouldStopForSquadMember();
}

void StealthRobot::updateSquadMemberDetection(float dt) {
    if (!m_squadMemberManager) return;

    // Call base class detection first
    Robot::updateSquadMemberDetection(dt);

    // If target detected, find the specific target to attack
    if (m_hasDetectedTarget && !isAttackingSquadMember() && !isDead()) {
        auto nearbyUnits = m_squadMemberManager->getActiveSquadMembersInRange(
            getPosition(), m_attackRange);

        SquadMember* closestTarget = nullptr;
        float closestDistance = m_attackRange + 1.0f;

        // Use directional filtering
        for (SquadMember* unit : nearbyUnits) {
            if (!unit || unit->isDestroyed()) continue;

            if (isSquadMemberInFront(unit)) {
                float distance = std::sqrt(
                    std::pow(getPosition().x - unit->getPosition().x, 2) +
                    std::pow(getPosition().y - unit->getPosition().y, 2)
                );

                if (distance < closestDistance) {
                    closestDistance = distance;
                    closestTarget = unit;
                }
            }
        }

        if (closestTarget) {
            onSquadMemberCollision(closestTarget);
            closestTarget->onRobotCollision(this);
        }
    }
}
void StealthRobot::attackSquadMember(SquadMember* target) {
    if (!target || isDead()) return;

    m_isAttacking = true;
    setMovementEnabled(false);
    switchToAttackAnimation();
}

void StealthRobot::updateAnimation(float dt) {
    Robot::updateAnimation(dt);

    auto& animSystem = AnimationSystem::getInstance();
    animSystem.updateEntityPosition(this, getPosition());

    if (!isDead()) {
        setFacingDirection(true);
    }

    if (isDead() && !m_deathAnimationStarted) {
        switchToDeathAnimation();
    }
}

void StealthRobot::switchToWalkingAnimation() {
    if (isDead()) return;

    auto& animSystem = AnimationSystem::getInstance();
    m_currentAnimationState = "walking";
    setMovementEnabled(true);
    m_isAttacking = false;

    if (animSystem.isEntityAnimationPaused(this)) {
        animSystem.resumeEntityAnimation(this);
    }
    else if (animSystem.hasAnimation("StealthRobotWalk")) {
        animSystem.playAnimation(this, "StealthRobotWalk");
    }
}

void StealthRobot::switchToAttackAnimation() {
    if (isDead()) return;
    AudioManager::getInstance().playSound("stealth_robot_hit");
    auto& animSystem = AnimationSystem::getInstance();
    m_currentAnimationState = "attacking";
    setMovementEnabled(false);
    m_isAttacking = true;

    if (animSystem.hasAnimation("StealthRobotHit")) {
        animSystem.playAnimation(this, "StealthRobotHit");
    }
    else if (animSystem.isEntityPlayingAnimation(this, "StealthRobotWalk")) {
        animSystem.pauseEntityAnimation(this);
    }
}

void StealthRobot::switchToDeathAnimation() {
    if (m_deathAnimationStarted) return;

    m_deathAnimationStarted = true;
    setMovementEnabled(false);
    m_isAttacking = false;
    AudioManager::getInstance().playSound("stealth_robot_die", AudioManager::AudioCategory::SFX, 0.2f);
    auto& animSystem = AnimationSystem::getInstance();
    m_currentAnimationState = "death";

    if (animSystem.hasAnimation("StealthRobotDead")) {
        animSystem.playAnimation(this, "StealthRobotDead");
        animSystem.setAnimationCompleteCallback(this, [this]() {
            setActive(false);
            });
    }
    else {
        setActive(false);
    }
}

ConfigLoader::RobotConfig StealthRobot::getConfig() const {
    return ConfigLoader::getInstance().loadRobotConfig("StealthRobot");
}