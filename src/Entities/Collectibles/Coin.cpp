#include "Entities/Collectibles/Coin.h"
#include "Systems/AnimationSystem.h"
#include "Managers/AudioManager.h"

Coin::Coin(int robotRewardValue)
    : Collectible(GameTypes::CollectibleType::Coin, robotRewardValue, Coin::VISUAL_DURATION)
{
    auto& animSystem = AnimationSystem::getInstance();
    animSystem.registerCollectible(this);
    AudioManager::getInstance().playSound("coin_added", AudioManager::AudioCategory::SFX, 0.5f);
    if (animSystem.hasAnimation("CoinSpin")) {
        animSystem.playAnimation(this, "CoinSpin");
    }
}

void Coin::update(float dt) {
    AnimationSystem::getInstance().updateEntityPosition(this, getPosition());

    if (m_autoCollected) {
        m_visualTimer += dt;
        if (m_visualTimer >= VISUAL_DURATION) {
            setActive(false); // Remove coin after visual effect
        }
    }
    else {
        Collectible::update(dt);
    }
}

void Coin::draw(sf::RenderWindow& window) const {
    if (!isActive()) return;

    if (m_autoCollected) {
        float alpha = 1.0f - (m_visualTimer / VISUAL_DURATION);
    }
}

void Coin::autoCollect() {
    if (m_autoCollected) return;

    m_autoCollected = true;
    m_visualTimer = 0.0f;
}

void Coin::applyEffect() {
}