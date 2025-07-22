#include "Entities/Robots/BasicRobot.h"
#include "Entities/Base/SquadMember.h"
#include "Managers/SquadMemberManager.h"
#include "Managers/AudioManager.h"
#include "Systems/AnimationSystem.h"
#include "Utils/ConfigLoader.h"
#include <cmath>

BasicRobot::BasicRobot()
    : Robot(GameTypes::RobotType::Basic, 100, 50.0f, 25)
    , m_deathAnimationStarted(false)
{
    setupFromConfig();
    initializeAnimations();
    setFacingDirection(true);
}

void BasicRobot::setupFromConfig() {
    // Load config once - Base Robot class handles common config loading
    auto& config = ConfigLoader::getInstance();

    try {
        m_health = config.getInt("BasicRobot", "health", 100);
        m_maxHealth = m_health;
        m_damage = config.getInt("BasicRobot", "damage", 25);
        m_rewardValue = config.getInt("BasicRobot", "reward", 30);
        m_originalSpeed = config.getFloat("BasicRobot", "speed", 50.0f);
        m_attackRange = config.getFloat("BasicRobot", "attackRange", 100.0f);
        m_attackDamage = m_damage;
    }
    catch (const std::exception&) {
        // Use constructor defaults
    }
}

void BasicRobot::initializeAnimations() {
    auto& animSystem = AnimationSystem::getInstance();
    animSystem.registerRobot(this);

    if (animSystem.hasAnimation("BasicRobotWalk")) {
        animSystem.playAnimation(this, "BasicRobotWalk");
    }
}

void BasicRobot::updateAnimation(float dt) {
    auto& animSystem = AnimationSystem::getInstance();
    animSystem.updateEntityPosition(this, getPosition());

    if (isMovementEnabled() && !isDead()) {
        setFacingDirection(true);
    }

    if (isDead() && !m_deathAnimationStarted) {
        AudioManager::getInstance().playSound("rock_robot_die", AudioManager::AudioCategory::SFX,  0.3f);
        m_deathAnimationStarted = true;

        if (animSystem.hasAnimation("BasicRobotDead")) {
            animSystem.playAnimation(this, "BasicRobotDead");
            animSystem.setAnimationCompleteCallback(this, [this]() {
                setActive(false);
                });
        }
        else {
            setActive(false);
        }
    }
}

bool BasicRobot::isPerformingSpecialAction() const {
    return isAttackingSquadMember() || shouldStopForSquadMember();
}

void BasicRobot::updateSquadMemberDetection(float dt) {
    if (!m_squadMemberManager) return;

    // Use base class detection
    Robot::updateSquadMemberDetection(dt);

    // Handle collision with detected targets
    if (m_hasDetectedTarget && !isAttackingSquadMember() && !isDead()) {
        auto nearbyUnits = m_squadMemberManager->getActiveSquadMembersInRange(
            getPosition(), m_attackRange);

        SquadMember* closestTarget = nullptr;
        float closestDistance = m_attackRange + 1.0f;

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

void BasicRobot::switchToAttackAnimation() {
    if (isDead()) return;

    auto& animSystem = AnimationSystem::getInstance();
    setMovementEnabled(false);
    AudioManager::getInstance().playSound("rock_robot_hit");
    if (animSystem.hasAnimation("BasicRobotHit")) {
        animSystem.playAnimation(this, "BasicRobotHit");
    }
    else {
        if (animSystem.isEntityPlayingAnimation(this, "BasicRobotWalk")) {
            animSystem.pauseEntityAnimation(this);
        }
    }
}

void BasicRobot::switchToWalkingAnimation() {
    if (isDead()) return;

    auto& animSystem = AnimationSystem::getInstance();
    setMovementEnabled(true);

    if (animSystem.isEntityAnimationPaused(this)) {
        animSystem.resumeEntityAnimation(this);
    }
    else if (animSystem.hasAnimation("BasicRobotWalk")) {
        animSystem.playAnimation(this, "BasicRobotWalk");
    }
}