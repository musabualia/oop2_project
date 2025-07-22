#include "Entities/SquadMembers/SniperMember.h"
#include "Entities/Base/Robot.h"
#include "Graphics/AnimationComponent.h"
#include "Systems/AnimationSystem.h"
#include "Managers/ProjectileManager.h"
#include "Managers/AudioManager.h"
#include "Utils/ConfigLoader.h"
#include <cmath>

SniperMember::SniperMember()
    : SquadMember(GameTypes::SquadMemberType::Sniper, 60, 250.0f, 50)
    , m_animationComponent(this)
    , m_attackDelay(2.5f)
    , m_canFire(true)
    , m_currentTarget(nullptr)
    , m_animationState(AnimationState::Idle)
    , m_visibleRobots(nullptr)
{
    initializeFromConfig();
    initializeAnimation();
    setupAnimationCallbacks();
    AnimationSystem::getInstance().registerSquadMember(this);
    m_attackTimer.setDuration(m_attackDelay);
    m_attackTimer.restart();
}

void SniperMember::initializeFromConfig() {
    initializeConfigFromName("Sniper", 2.5f);

    auto& config = ConfigLoader::getInstance();
    m_damage = config.getInt("SniperBullet", "damage", 40);
}

void SniperMember::initializeAnimation() {
    auto& animSystem = AnimationSystem::getInstance();
    if (const Animation* idleAnim = animSystem.getAnimation("SniperIdle")) {
        m_animationComponent.play(*idleAnim, "SniperIdle");
        m_animationState = AnimationState::Idle;
    }
}

void SniperMember::setupAnimationCallbacks() {
    m_animationComponent.setAnimationCompleteCallback([this]() {
        if (m_animationState == AnimationState::Shooting) {
            returnToIdleAnimation();
        }
        else if (m_animationState == AnimationState::Dead) {
            setActive(false);
        }
        });
}

void SniperMember::update(float dt) {
    SquadMember::update(dt);
    m_animationComponent.update(dt);

    if (isDestroyed() || m_isDying) {
        return;
    }

    updateTargeting(dt);
    updateFiring(dt);
    updateAnimation(dt);
}

void SniperMember::updateTargeting(float dt) {
    m_currentTarget = nullptr;
    if (m_visibleRobots) {
        m_currentTarget = findTarget(*m_visibleRobots);
    }
}

void SniperMember::updateFiring(float dt) {
    m_attackTimer.update(dt);
    if (m_attackTimer.isElapsed()) {
        m_canFire = true;
    }

    if (m_canFire && hasValidTarget()) {
        fireBullet();
        m_canFire = false;
        m_attackTimer.restart();
    }
}

void SniperMember::updateAnimation(float dt) {
    m_animationComponent.setPosition(getPosition());
}

bool SniperMember::hasValidTarget() const {
    return m_currentTarget &&
        m_currentTarget->isActive() &&
        !m_currentTarget->isDead() &&
        distanceToTarget(m_currentTarget) <= m_range;
}

void SniperMember::fireBullet() {
    if (!hasValidTarget()) return;

    startShootingAnimation();
    AudioManager::getInstance().playSound("sniper_shot", AudioManager::AudioCategory::SFX, 0.3f);

    auto& projectileManager = ProjectileManager::getInstance();
    projectileManager.fireProjectile(
        GameTypes::ProjectileType::SniperBullet,
        0,
        m_currentTarget,
        this
    );
}

void SniperMember::startShootingAnimation() {
    if (m_animationState == AnimationState::Shooting) return;

    auto& animSystem = AnimationSystem::getInstance();
    if (const Animation* shootAnim = animSystem.getAnimation("SniperShoot")) {
        m_animationComponent.play(*shootAnim, "SniperShoot");
        m_animationState = AnimationState::Shooting;
    }
}

void SniperMember::returnToIdleAnimation() {
    if (m_animationState == AnimationState::Idle) return;

    auto& animSystem = AnimationSystem::getInstance();
    if (const Animation* idleAnim = animSystem.getAnimation("SniperIdle")) {
        m_animationComponent.play(*idleAnim, "SniperIdle");
        m_animationState = AnimationState::Idle;
    }
}

void SniperMember::draw(sf::RenderWindow& window) const {
    m_animationComponent.setPosition(getPosition());
    m_animationComponent.draw(window);
    SquadMember::draw(window);
}

void SniperMember::attack(const std::vector<std::unique_ptr<Robot>>& robots) {
    m_visibleRobots = &robots;
}

Robot* SniperMember::findTarget(const std::vector<std::unique_ptr<Robot>>& robots) const {
    Robot* strongestTarget = nullptr;
    int highestHealth = 0;

    for (const auto& robot : robots) {
        if (!robot || !robot->isActive() || robot->isDead()) continue;

        sf::Vector2f robotPos = robot->getPosition();
        sf::Vector2f myPos = getPosition();

        if (robotPos.x <= myPos.x) continue;

        float distance = distanceToTarget(robot.get());
        float yDifference = std::abs(robotPos.y - myPos.y);
        if (yDifference > 60.0f) continue;

        if (distance <= m_range && robot->getHealth() > highestHealth) {
            highestHealth = robot->getHealth();
            strongestTarget = robot.get();
        }
    }

    return strongestTarget;
}

void SniperMember::setNearbyRobots(const std::vector<std::unique_ptr<Robot>>* robots) {
    m_visibleRobots = robots;
}

void SniperMember::playDeathAnimation() {
    if (m_animationState == AnimationState::Dead) return;
    AudioManager::getInstance().playSound("squad_died");
    auto& animSystem = AnimationSystem::getInstance();
    if (const Animation* deathAnim = animSystem.getAnimation("SniperDead")) {
        m_animationComponent.play(*deathAnim, "SniperDead");
        m_animationState = AnimationState::Dead;
    }
    else {
        setActive(false);
    }
}

ConfigLoader::SquadMemberConfig SniperMember::getConfig() const {
    return ConfigLoader::getInstance().loadSquadMemberConfig("Sniper");
}