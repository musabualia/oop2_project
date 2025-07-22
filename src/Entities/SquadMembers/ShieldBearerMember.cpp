#include "Entities/SquadMembers/ShieldBearerMember.h"
#include "Entities/Base/Robot.h"
#include "Graphics/AnimationComponent.h"
#include "Systems/AnimationSystem.h"
#include "Managers/AudioManager.h"
#include "Utils/ConfigLoader.h"
#include <cmath>

ShieldBearerMember::ShieldBearerMember()
    : SquadMember(GameTypes::SquadMemberType::ShieldBearer, 50, 100.0f, 15)
    , m_animationComponent(this)
    , m_animationState(AnimationState::Idle)
    , m_visibleRobots(nullptr)
    , m_robotNear(false)
{
    initializeFromConfig();
    initializeAnimation();
    setupAnimationCallbacks();
    AnimationSystem::getInstance().registerSquadMember(this);
}

void ShieldBearerMember::initializeFromConfig() {
    initializeConfigFromName("ShieldBearer", 0.0f);
}

void ShieldBearerMember::initializeAnimation() {
    auto& animSystem = AnimationSystem::getInstance();
    if (const Animation* idleAnim = animSystem.getAnimation("ShieldBearerIdle")) {
        m_animationComponent.play(*idleAnim, "ShieldBearerIdle");
        m_animationState = AnimationState::Idle;
    }
}

void ShieldBearerMember::setupAnimationCallbacks() {
    m_animationComponent.setAnimationCompleteCallback([this]() {
        if (m_animationState == AnimationState::Blocking) {
            returnToIdleAnimation();
        }
        else if (m_animationState == AnimationState::Dead) {
            setActive(false);
        }
        });
}

void ShieldBearerMember::update(float dt) {
    SquadMember::update(dt);
    m_animationComponent.update(dt);

    if (isDestroyed() || m_isDying) {
        return;
    }

    updateRobotDetection(dt);
    updateAnimation(dt);
}

void ShieldBearerMember::updateRobotDetection(float dt) {
    bool robotDetected = false;

    if (m_visibleRobots) {
        for (const auto& robot : *m_visibleRobots) {
            if (!robot || !robot->isActive() || robot->isDead()) continue;

            float distance = distanceToTarget(robot.get());
            sf::Vector2f robotPos = robot->getPosition();
            sf::Vector2f myPos = getPosition();
            float yDifference = std::abs(robotPos.y - myPos.y);

            if (yDifference > 60.0f) continue;

            if (distance <= m_range && robotPos.x > myPos.x) {
                robotDetected = true;
                break;
            }
        }
    }

    if (robotDetected && !m_robotNear) {
        m_robotNear = true;
        switchToBlockingAnimation();
        AudioManager::getInstance().playSound("shield_barier_fight", AudioManager::AudioCategory::SFX);
    }
    else if (!robotDetected && m_robotNear) {
        m_robotNear = false;
        returnToIdleAnimation();
    }
}

void ShieldBearerMember::updateAnimation(float dt) {
    m_animationComponent.setPosition(getPosition());
}

void ShieldBearerMember::switchToBlockingAnimation() {
    if (m_animationState == AnimationState::Blocking) return;

    auto& animSystem = AnimationSystem::getInstance();
    if (const Animation* blockAnim = animSystem.getAnimation("ShieldBearerBlock")) {
        m_animationComponent.play(*blockAnim, "ShieldBearerBlock");
        m_animationState = AnimationState::Blocking;
    }
}

void ShieldBearerMember::returnToIdleAnimation() {
    if (m_animationState == AnimationState::Idle) return;

    auto& animSystem = AnimationSystem::getInstance();
    if (const Animation* idleAnim = animSystem.getAnimation("ShieldBearerIdle")) {
        m_animationComponent.play(*idleAnim, "ShieldBearerIdle");
        m_animationState = AnimationState::Idle;
    }
}

void ShieldBearerMember::draw(sf::RenderWindow& window) const {
    m_animationComponent.setPosition(getPosition());
    m_animationComponent.draw(window);
    SquadMember::draw(window);
}

void ShieldBearerMember::attack(const std::vector<std::unique_ptr<Robot>>& robots) {
    m_visibleRobots = &robots;
}

Robot* ShieldBearerMember::findTarget(const std::vector<std::unique_ptr<Robot>>& robots) const {
    return nullptr;
}

void ShieldBearerMember::setNearbyRobots(const std::vector<std::unique_ptr<Robot>>* robots) {
    m_visibleRobots = robots;
}

void ShieldBearerMember::playDeathAnimation() {
    if (m_animationState == AnimationState::Dead) return;
    AudioManager::getInstance().playSound("shield_died");
    auto& animSystem = AnimationSystem::getInstance();
    if (const Animation* deathAnim = animSystem.getAnimation("ShieldBearerDead")) {
        m_animationComponent.play(*deathAnim, "ShieldBearerDead");
        m_animationState = AnimationState::Dead;
    }
    else {
        setActive(false);
    }
}

ConfigLoader::SquadMemberConfig ShieldBearerMember::getConfig() const {
    return ConfigLoader::getInstance().loadSquadMemberConfig("ShieldBearer");
}