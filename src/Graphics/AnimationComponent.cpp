// ================================
// src/Graphics/AnimationComponent.cpp 
// ================================
#include "Graphics/AnimationComponent.h"
#include "Graphics/Animation.h" 
#include "Core/Constants.h" 

AnimationComponent::AnimationComponent(void* owner)
    : m_owner(owner) {
}

void AnimationComponent::play(const Animation& animation, const std::string& animationName) {
    m_currentAnimation = &animation;
    m_currentAnimationName = animationName;
    m_time = 0.f;
    m_currentFrame = 0;
    m_previousFrame = -1;

    m_sprite.setTexture(animation.getTexture());
    m_sprite.setTextureRect(animation.getFrame(0));

    const auto& frame = animation.getFrame(0);
    m_sprite.setOrigin(frame.width / 2.f, frame.height / 2.f);

    float targetSize = GameConstants::GRID_CELL_HEIGHT;
    float baseScale = targetSize / std::max(frame.width, frame.height);

    bool shouldFlipHorizontally = needsHorizontalFlip();
    float scaleX = shouldFlipHorizontally ? -baseScale : baseScale;
    float scaleY = baseScale;

    m_sprite.setScale(scaleX, scaleY);
}

void AnimationComponent::update(float dt) {
    if (!m_currentAnimation || m_paused) return;

    float adjustedDt = dt * m_animationSpeed;
    m_time += adjustedDt;

    float frameTime = m_currentAnimation->getDuration() / m_currentAnimation->getFrameCount();

    int newFrame = static_cast<int>((m_time / frameTime)) % m_currentAnimation->getFrameCount();

    bool isDeathAnimation = m_currentAnimationName.find("Death") != std::string::npos ||
        m_currentAnimationName.find("Dead") != std::string::npos;

    bool animationCompleted = false;

    if (isDeathAnimation) {
        float totalTime = m_currentAnimation->getDuration();
        if (m_time >= totalTime) {
            newFrame = m_currentAnimation->getFrameCount() - 1;
            animationCompleted = true;
        }
    }
    else {
        if (m_time >= m_currentAnimation->getDuration()) {
            animationCompleted = true;
        }
    }

    if (newFrame != m_currentFrame) {
        m_previousFrame = m_currentFrame;
        m_currentFrame = newFrame;
        m_sprite.setTextureRect(m_currentAnimation->getFrame(m_currentFrame));
        checkFrameCallbacks();
    }

    if (animationCompleted) {
        checkAnimationComplete();
    }
}

void AnimationComponent::draw(sf::RenderWindow& window) const {
    if (m_currentAnimation && m_sprite.getTexture()) {
        window.draw(m_sprite);
    }
}

void AnimationComponent::setPosition(const sf::Vector2f& pos) {
    sf::Vector2f adjustedPos = pos;
    adjustedPos.y -= 35.0f;
    m_sprite.setPosition(adjustedPos);
}

bool AnimationComponent::needsHorizontalFlip() const {
    if (m_currentAnimationName.find("FireRobot") != std::string::npos) {
        return true;
    }

    if (m_currentAnimationName.find("BasicRobot") != std::string::npos ||
        m_currentAnimationName.find("RockRobot") != std::string::npos) {
        return true;
    }

    if (m_currentAnimationName.find("StealthRobot") != std::string::npos) {
        return true;
    }

    return false;
}

bool AnimationComponent::isLastFrame() const {
    if (!m_currentAnimation) return false;
    return m_currentFrame == (m_currentAnimation->getFrameCount() - 1);
}

bool AnimationComponent::isAnimationComplete() const {
    if (!m_currentAnimation) return true;

    float totalDuration = m_currentAnimation->getDuration();
    return m_time >= totalDuration;
}

void AnimationComponent::setFrameCallback(int frameIndex, FrameCallback callback) {
    m_frameCallbacks[frameIndex] = callback;
}

void AnimationComponent::setAnimationCompleteCallback(AnimationCallback callback) {
    m_animationCompleteCallback = callback;
}

void AnimationComponent::clearCallbacks() {
    m_frameCallbacks.clear();
    m_animationCompleteCallback = nullptr;
}

void AnimationComponent::checkFrameCallbacks() {
    auto it = m_frameCallbacks.find(m_currentFrame);
    if (it != m_frameCallbacks.end() && it->second) {
        it->second(m_currentFrame);
    }
}

void AnimationComponent::checkAnimationComplete() {
    if (m_animationCompleteCallback) {
        m_animationCompleteCallback();
    }
}
