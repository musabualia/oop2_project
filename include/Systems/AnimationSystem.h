#pragma once
#include "Graphics/AnimationComponent.h"
#include "Core/Constants.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

class SquadMember;
class Robot;
class Collectible;
class Projectile;

class AnimationSystem {
public:
    // Singleton access
    static AnimationSystem& getInstance();

    // System lifecycle
    void initialize();
    void shutdown();
    void update(float dt);

    // Animation library management
    bool loadAnimationFromFile(const std::string& filename);
    void createAnimation(const std::string& name, const Animation& animation);
    const Animation* getAnimation(const std::string& name) const;
    bool hasAnimation(const std::string& name) const;

    // Entity registration (just assigns AnimationComponents)
    void registerSquadMember(SquadMember* unit);
    void registerRobot(Robot* robot);
    void registerCollectible(Collectible* collectible);
    void registerProjectile(Projectile* projectile);

    void unregisterEntity(void* entity);

    // Quick animation triggers (delegates to AnimationComponent)
    void playAnimation(void* entity, const std::string& animationName);
    void playDamageFlash(void* entity);
    void playUpgradeEffect(SquadMember* unit);

    // ✅ NEW: Animation callback support
    void setAnimationCompleteCallback(void* entity, AnimationComponent::AnimationCallback callback);

    // Global control
    void setGlobalSpeed(float speed);
    float getGlobalSpeed() const;

    // Predefined animations creation
    void createGameAnimations();

    void render(sf::RenderWindow& window);

    // Debug helper
    void updateEntityPosition(void* entity, const sf::Vector2f& position);

    // Animation control methods
    void pauseEntityAnimation(void* entity);
    void resumeEntityAnimation(void* entity);
    bool isEntityAnimationPaused(void* entity) const;
    bool isEntityPlayingAnimation(void* entity, const std::string& animationName) const;

private:
    AnimationSystem() = default;
    ~AnimationSystem() = default;
    AnimationSystem(const AnimationSystem&) = delete;
    AnimationSystem& operator=(const AnimationSystem&) = delete;

    std::unordered_map<std::string, std::unique_ptr<Animation>> m_animations;
    std::unordered_map<void*, std::unique_ptr<AnimationComponent>> m_entityComponents;
    float m_globalSpeed = 1.0f;

    void updateComponents(float dt);

    void createRobotAnimations();
    void createSquadMemberAnimations();
    void createProjectileAnimations();
    void createCollectibleAnimations();
    void createEffectAnimations();
};