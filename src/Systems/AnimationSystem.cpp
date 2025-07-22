// ================================
// src/Systems/AnimationSystem.cpp - UPDATED with New Robot Hit Animations
// ================================
#include "Systems/AnimationSystem.h"
#include "Managers/ResourceManager.h"
#include "Entities/SquadMembers/HeavyGunnerMember.h"
#include "Entities/Robots/BasicRobot.h"
#include <iostream>

AnimationSystem& AnimationSystem::getInstance() {
    static AnimationSystem instance;
    return instance;
}

void AnimationSystem::initialize() {
    createGameAnimations();
}

void AnimationSystem::shutdown() {
    m_animations.clear();
    m_entityComponents.clear();
}

void AnimationSystem::update(float dt) {
    updateComponents(dt * m_globalSpeed);
}

void AnimationSystem::updateComponents(float dt) {
    for (auto& [entity, component] : m_entityComponents) {
        if (component) {
            component->update(dt);
        }
    }
}

bool AnimationSystem::loadAnimationFromFile(const std::string& filename) {
    return false;
}

// MAIN METHOD: Now calls split methods
void AnimationSystem::createGameAnimations() {
    createRobotAnimations();
    createSquadMemberAnimations();
    createProjectileAnimations();
    createCollectibleAnimations();
    createEffectAnimations();
}

// ✅ UPDATED: Robot animations with new hit animations
void AnimationSystem::createRobotAnimations() {
    auto& rm = ResourceManager::getInstance();
    // ===== BASIC ROBOT - UPDATED WITH HIT ANIMATION (CROPPED FRAME SIZES) =====
    const int frameWidth = 129;
    const int frameHeight = 124;
    const int frameCount = 10;

    // Walk
    if (rm.hasTexture("RockRobot_Walk.png")) {
        const auto& tex = rm.getTexture("RockRobot_Walk.png");
        Animation anim(tex, frameWidth, frameHeight, frameCount, 2.0f);
        createAnimation("BasicRobotWalk", anim);
    }

    // Death
    if (rm.hasTexture("RobotRock_Dead.png")) {
        const auto& tex = rm.getTexture("RobotRock_Dead.png");
        Animation anim(tex, frameWidth, frameHeight, frameCount, 1.5f);
        createAnimation("BasicRobotDead", anim);
    }

    // Hit
    if (rm.hasTexture("RockRobot_Hit.png")) {
        const auto& tex = rm.getTexture("RockRobot_Hit.png");
        Animation anim(tex, frameWidth, frameHeight, frameCount, 2.0f);
        createAnimation("BasicRobotHit", anim);
    }


    // ===== STEALTH ROBOT - UPDATED WITH NEW ANIMATIONS =====
    const int sframeWidth = 198;
    const int sframeHeight = 181;
    const int sframeCount = 10;

    // ✅ StealthRobot Walk Animation
    if (rm.hasTexture("StealthRobot_Walk1.png")) {
        const auto& tex = rm.getTexture("StealthRobot_Walk1.png");
        Animation walkAnim(tex, sframeWidth, sframeHeight, sframeCount, 2.0f);
        createAnimation("StealthRobotWalk", walkAnim);
    }

    //  StealthRobot Hit Animation 
    if (rm.hasTexture("StealthRobot_Hit.png")) {
        const auto& tex = rm.getTexture("StealthRobot_Hit.png");
        Animation hitAnim(tex, tex.getSize().x / 10, tex.getSize().y, 10, 1.2f); // only 5 frames
        createAnimation("StealthRobotHit", hitAnim);
    }

    //  StealthRobot Death Animation
    if (rm.hasTexture("StealthRobot_Dead.png")) {
        const auto& tex = rm.getTexture("StealthRobot_Dead.png");
        Animation anim(tex, tex.getSize().x / 10, tex.getSize().y, 10, 1.2f);
        createAnimation("StealthRobotDead", anim);
    }
  


    // ===== FIRE ROBOT (UNCHANGED) =====
    if (rm.hasTexture("fire_robot_walk.png")) {
        const auto& tex = rm.getTexture("fire_robot_walk.png");
        // 10 frames, dimensions 1059x91, each frame ~106x91
        Animation walkAnim(tex, tex.getSize().x / 10, tex.getSize().y, 10, 2.0f);
        createAnimation("FireRobotWalk", walkAnim);
    }

    if (rm.hasTexture("fire_robot_death.png")) {
        const auto& tex = rm.getTexture("fire_robot_death.png");
        // 6 frames, dimensions 608x91, each frame ~101x91  
        Animation deathAnim(tex, tex.getSize().x / 10, tex.getSize().y, 10, 1.2f);
        createAnimation("FireRobotDeath", deathAnim);
    }


    if (rm.hasTexture("fire_robot.png")) {
        const auto& tex = rm.getTexture("fire_robot.png");
        // Single static texture for when robot stops
        Animation staticAnim(tex, tex.getSize().x, tex.getSize().y, 1, 1.5f);
        createAnimation("FireRobotStatic", staticAnim);
    }
}

void AnimationSystem::createSquadMemberAnimations() {
    auto& rm = ResourceManager::getInstance();

    // ===== HEAVY GUNNER =====
    if (rm.hasTexture("HeavyGunner_Idle.png")) {
        const auto& tex = rm.getTexture("HeavyGunner_Idle.png");
        // 717x130, 5 frames = 143.4x130 per frame
        Animation anim(tex, tex.getSize().x / 5, tex.getSize().y, 5, 1.0f);
        createAnimation("HeavyGunnerIdle", anim);
    }

    if (rm.hasTexture("HeavyGunner_Shot.png")) {
        const auto& tex = rm.getTexture("HeavyGunner_Shot.png");
        // 717x130, 5 frames = 143.4x130 per frame
        Animation anim(tex, tex.getSize().x / 5, tex.getSize().y, 5, 0.6f);
        createAnimation("HeavyGunnerShoot", anim);
    }

    // HeavyGunner Death Animation
    if (rm.hasTexture("HeavyGunner_Dead.png")) {
        const auto& tex = rm.getTexture("HeavyGunner_Dead.png");
        // 717x130, 5 frames = 143.4x130 per frame
        Animation anim(tex, tex.getSize().x / 5, tex.getSize().y, 5, 0.5f);  // Longer duration for death
        createAnimation("HeavyGunnerDead", anim);
    }

    // ===== SNIPER =====
    if (rm.hasTexture("ss_Idle.png")) {
        const auto& tex = rm.getTexture("ss_Idle.png");
        // Assuming 6 frames in one row
        Animation anim(tex, tex.getSize().x / 5, tex.getSize().y, 5, 1.0f);
        createAnimation("SniperIdle", anim);
    }

    if (rm.hasTexture("Sniper_Shot.png")) {
        const auto& tex = rm.getTexture("Sniper_Shot.png");
        // 755x154, 5 frames = 151x154 per frame
        Animation anim(tex, tex.getSize().x / 5, tex.getSize().y, 5, 1.0f);
        createAnimation("SniperShoot", anim);
    }

    // Sniper Death Animation
    if (rm.hasTexture("Sniper_Dead.png")) {
        const auto& tex = rm.getTexture("Sniper_Dead.png");
        // 755x154, 5 frames = 151x154 per frame
        Animation anim(tex, tex.getSize().x / 5, tex.getSize().y, 5, 0.5f);  // Longer duration for death
        createAnimation("SniperDead", anim);
    }

    // ===== SHIELD BEARER =====
    if (rm.hasTexture("ShieldBearer_Idle.png")) {
        const auto& tex = rm.getTexture("ShieldBearer_Idle.png");
        // Assuming similar format to others
        Animation anim(tex, tex.getSize().x / 5, tex.getSize().y, 5, 1.0f);
        createAnimation("ShieldBearerIdle", anim);
    }

    if (rm.hasTexture("ShieldBearer_Block.png")) {
        const auto& tex = rm.getTexture("ShieldBearer_Block.png");
        // Block animation (used as hit/defense animation)
        Animation anim(tex, tex.getSize().x / 5, tex.getSize().y, 5, 1.0f);
        createAnimation("ShieldBearerBlock", anim);
    }

    // ShieldBearer Death Animation
    if (rm.hasTexture("ShieldBearer_Dead.png")) {
        const auto& tex = rm.getTexture("ShieldBearer_Dead.png");
        // 615x124, 5 frames = 123x124 per frame
        Animation anim(tex, tex.getSize().x / 5, tex.getSize().y, 5, 0.5f);  // Longer duration for death
        createAnimation("ShieldBearerDead", anim);
    }
}

void AnimationSystem::createProjectileAnimations() {
    auto& rm = ResourceManager::getInstance();
}

// Collectible animations only
void AnimationSystem::createCollectibleAnimations() {
    auto& rm = ResourceManager::getInstance();

    // ===== COIN ANIMATION =====
    if (rm.hasTexture("coin.png")) {
        const auto& tex = rm.getTexture("coin.png");
        int frameCount = 4;
        int frameWidth = tex.getSize().x / frameCount;
        int frameHeight = tex.getSize().y;

        // Coin spinning animation - loops continuously
        Animation coinAnim(tex, frameWidth, frameHeight, frameCount, 2.0f);
        createAnimation("CoinSpin", coinAnim);
    }

    // ===== HEALTH BAG ANIMATION =====
    if (rm.hasTexture("HealthBag.png")) {
        const auto& tex = rm.getTexture("HealthBag.png");
        int frameCount = 5;
        int frameWidth = tex.getSize().x / frameCount;
        int frameHeight = tex.getSize().y;

        // Health bag idle animation - loops continuously
        Animation healthBagAnim(tex, frameWidth, frameHeight, frameCount, 2.0f);
        createAnimation("HealthBag", healthBagAnim);
    }
}

// Effect animations only
void AnimationSystem::createEffectAnimations() {
    auto& rm = ResourceManager::getInstance();

    // ===== EXPLOSION ANIMATION =====
    if (rm.hasTexture("explosion.png")) {
        const auto& tex = rm.getTexture("explosion.png");
        int columns = 4;
        int frameWidth = tex.getSize().x / columns;
        int frameHeight = tex.getSize().y;
        Animation anim(tex, frameWidth, frameHeight, columns, 0.4f);
        createAnimation("Explosion", anim);
    }
}

void AnimationSystem::render(sf::RenderWindow& window) {
    for (const auto& [entity, component] : m_entityComponents) {
        if (component) component->draw(window);
    }
}

void AnimationSystem::updateEntityPosition(void* entity, const sf::Vector2f& position) {
    auto it = m_entityComponents.find(entity);
    if (it != m_entityComponents.end() && it->second) {
        it->second->setPosition(position);
    }
}

void AnimationSystem::createAnimation(const std::string& name, const Animation& animation) {
    m_animations[name] = std::make_unique<Animation>(animation);
}

const Animation* AnimationSystem::getAnimation(const std::string& name) const {
    auto it = m_animations.find(name);
    return (it != m_animations.end()) ? it->second.get() : nullptr;
}

bool AnimationSystem::hasAnimation(const std::string& name) const {
    return m_animations.find(name) != m_animations.end();
}

void AnimationSystem::registerSquadMember(SquadMember* unit) {
    m_entityComponents[unit] = std::make_unique<AnimationComponent>(unit);
}

void AnimationSystem::registerRobot(Robot* robot) {
    m_entityComponents[robot] = std::make_unique<AnimationComponent>(robot);
}

void AnimationSystem::registerCollectible(Collectible* collectible) {
    m_entityComponents[collectible] = std::make_unique<AnimationComponent>(collectible);
}

void AnimationSystem::registerProjectile(Projectile* projectile) {
    m_entityComponents[projectile] = std::make_unique<AnimationComponent>(projectile);
}

void AnimationSystem::unregisterEntity(void* entity) {
    m_entityComponents.erase(entity);
}

void AnimationSystem::playAnimation(void* entity, const std::string& animationName) {
    auto it = m_entityComponents.find(entity);
    if (it != m_entityComponents.end()) {
        const Animation* anim = getAnimation(animationName);
        if (anim) {
            it->second->play(*anim, animationName);
        }
    }
}

// Animation callback support
void AnimationSystem::setAnimationCompleteCallback(void* entity, AnimationComponent::AnimationCallback callback) {
    auto it = m_entityComponents.find(entity);
    if (it != m_entityComponents.end() && it->second) {
        it->second->setAnimationCompleteCallback(callback);
    }
}

void AnimationSystem::playDamageFlash(void* entity) {
    // Reserved for future use
}

void AnimationSystem::playUpgradeEffect(SquadMember* unit) {
    playAnimation(unit, "Upgrade");
}

void AnimationSystem::setGlobalSpeed(float speed) {
    m_globalSpeed = speed;
}

float AnimationSystem::getGlobalSpeed() const {
    return m_globalSpeed;
}

void AnimationSystem::pauseEntityAnimation(void* entity) {
    auto it = m_entityComponents.find(entity);
    if (it != m_entityComponents.end() && it->second) {
        it->second->pause();
    }
}

void AnimationSystem::resumeEntityAnimation(void* entity) {
    auto it = m_entityComponents.find(entity);
    if (it != m_entityComponents.end() && it->second) {
        it->second->resume();
    }
}

bool AnimationSystem::isEntityAnimationPaused(void* entity) const {
    auto it = m_entityComponents.find(entity);
    if (it != m_entityComponents.end() && it->second) {
        return it->second->isPaused();
    }
    return false;
}

bool AnimationSystem::isEntityPlayingAnimation(void* entity, const std::string& animationName) const {
    auto it = m_entityComponents.find(entity);
    if (it != m_entityComponents.end() && it->second) {
        return it->second->isPlayingAnimation(animationName);
    }
    return false;
}