#include "Entities/SquadMembers/HeavyGunnerMember.h"
#include "Entities/Base/Robot.h"
#include "Graphics/AnimationComponent.h"
#include "Systems/AnimationSystem.h"
#include "Managers/ProjectileManager.h"
#include "Managers/AudioManager.h"
#include "Utils/ConfigLoader.h"
#include <cmath>

HeavyGunnerMember::HeavyGunnerMember()
    : SquadMember(GameTypes::SquadMemberType::HeavyGunner, 40, 200.0f, 25)
    , m_animationComponent(this)
    , m_attackDelay(1.5f)
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

void HeavyGunnerMember::initializeFromConfig() {
    initializeConfigFromName("HeavyGunner", 1.5f);

    auto& config = ConfigLoader::getInstance();
    m_damage = config.getInt("SquadBullet", "damage", 25);
}

void HeavyGunnerMember::initializeAnimation() {
    auto& animSystem = AnimationSystem::getInstance();
    if (const Animation* idleAnim = animSystem.getAnimation("HeavyGunnerIdle")) {
        m_animationComponent.play(*idleAnim, "HeavyGunnerIdle");
        m_animationState = AnimationState::Idle;
    }
}

void HeavyGunnerMember::setupAnimationCallbacks() {
    m_animationComponent.setAnimationCompleteCallback([this]() {
        if (m_animationState == AnimationState::Shooting) {
            returnToIdleAnimation();
        }
        else if (m_animationState == AnimationState::Dead) {
            setActive(false);
        }
        });
}

void HeavyGunnerMember::update(float dt) {
    SquadMember::update(dt);
    m_animationComponent.update(dt);

    if (isDestroyed() || m_isDying) {
        return;
    }

    updateTargeting(dt);
    updateFiring(dt);
    updateAnimation(dt);
}

void HeavyGunnerMember::updateTargeting(float dt) {
    m_currentTarget = nullptr;
    if (m_visibleRobots) {
        m_currentTarget = findTarget(*m_visibleRobots);
    }
}

void HeavyGunnerMember::updateFiring(float dt) {
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

void HeavyGunnerMember::updateAnimation(float dt) {
    m_animationComponent.setPosition(getPosition());
}

bool HeavyGunnerMember::hasValidTarget() const {
    return m_currentTarget &&
        m_currentTarget->isActive() &&
        !m_currentTarget->isDead() &&
        distanceToTarget(m_currentTarget) <= m_range;
}

void HeavyGunnerMember::fireBullet() {
    if (!hasValidTarget()) return;

    startShootingAnimation();
    AudioManager::getInstance().playSound("heavy_gunner_shot", AudioManager::AudioCategory::SFX, 0.3f);

    auto& projectileManager = ProjectileManager::getInstance();
    projectileManager.fireProjectile(
        GameTypes::ProjectileType::Bullet,
        0,
        m_currentTarget,
        this
    );
}

void HeavyGunnerMember::startShootingAnimation() {
    if (m_animationState == AnimationState::Shooting) return;

    auto& animSystem = AnimationSystem::getInstance();
    if (const Animation* shootAnim = animSystem.getAnimation("HeavyGunnerShoot")) {
        m_animationComponent.play(*shootAnim, "HeavyGunnerShoot");
        m_animationState = AnimationState::Shooting;
    }
}

void HeavyGunnerMember::returnToIdleAnimation() {
    if (m_animationState == AnimationState::Idle) return;

    auto& animSystem = AnimationSystem::getInstance();
    if (const Animation* idleAnim = animSystem.getAnimation("HeavyGunnerIdle")) {
        m_animationComponent.play(*idleAnim, "HeavyGunnerIdle");
        m_animationState = AnimationState::Idle;
    }
}

void HeavyGunnerMember::draw(sf::RenderWindow& window) const {
    m_animationComponent.setPosition(getPosition());
    m_animationComponent.draw(window);
    SquadMember::draw(window);
}

void HeavyGunnerMember::attack(const std::vector<std::unique_ptr<Robot>>& robots) {
    m_visibleRobots = &robots;
}

Robot* HeavyGunnerMember::findTarget(const std::vector<std::unique_ptr<Robot>>& robots) const {
    Robot* closestTarget = nullptr;
    float closestDistance = m_range + 1.0f;

    for (const auto& robot : robots) {
        if (!robot || !robot->isActive() || robot->isDead()) continue;

        sf::Vector2f robotPos = robot->getPosition();
        sf::Vector2f myPos = getPosition();

        if (robotPos.x <= myPos.x) continue;

        float distance = distanceToTarget(robot.get());
        float yDifference = std::abs(robotPos.y - myPos.y);
        if (yDifference > 60.0f) continue;

        if (distance <= m_range && distance < closestDistance) {
            closestDistance = distance;
            closestTarget = robot.get();
        }
    }

    return closestTarget;
}

void HeavyGunnerMember::setNearbyRobots(const std::vector<std::unique_ptr<Robot>>* robots) {
    m_visibleRobots = robots;
}

void HeavyGunnerMember::playDeathAnimation() {
    if (m_animationState == AnimationState::Dead) return;
    AudioManager::getInstance().playSound("squad_died");
    auto& animSystem = AnimationSystem::getInstance();
    if (const Animation* deathAnim = animSystem.getAnimation("HeavyGunnerDead")) {
        m_animationComponent.play(*deathAnim, "HeavyGunnerDead");
        m_animationState = AnimationState::Dead;
    }
    else {
        setActive(false);
    }
}

ConfigLoader::SquadMemberConfig HeavyGunnerMember::getConfig() const {
    return ConfigLoader::getInstance().loadSquadMemberConfig("HeavyGunner");
}