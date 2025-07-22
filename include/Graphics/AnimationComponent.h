// ================================
// include/Graphics/AnimationComponent.h - PROPER FIX: Sprite flipping approach
// ================================
#pragma once
#include "Graphics/Animation.h"
#include <SFML/Graphics.hpp>
#include <functional>
#include <map>

class AnimationComponent {
public:
    // Callback type for animation events
    using FrameCallback = std::function<void(int frameIndex)>;
    using AnimationCallback = std::function<void()>;

    explicit AnimationComponent(void* owner);                       // Constructor with owner reference
    void play(const Animation& animation, const std::string& animationName = ""); // Play animation
    void update(float dt);                                          // Update animation timing
    void draw(sf::RenderWindow& window) const;                      // Draw current frame
    void setPosition(const sf::Vector2f& pos);                      // Set sprite position

    // Frame detection methods
    int getCurrentFrame() const { return m_currentFrame; }          // Get current frame index
    bool isOnFrame(int frameIndex) const { return m_currentFrame == frameIndex; } // Check frame
    bool isLastFrame() const;                                       // Check if on last frame
    bool isFirstFrame() const { return m_currentFrame == 0; }       // Check if on first frame

    // Animation state queries
    bool isAnimationComplete() const;                              // Check if animation complete
    const std::string& getCurrentAnimationName() const { return m_currentAnimationName; } // Current anim name
    bool isPlayingAnimation(const std::string& name) const { return m_currentAnimationName == name; } // Is playing specific anim

    // Callback system
    void setFrameCallback(int frameIndex, FrameCallback callback);  // Set callback for specific frame
    void setAnimationCompleteCallback(AnimationCallback callback); // Callback on complete
    void clearCallbacks();                                          // Clear all callbacks

    // Animation control
    void pause() { m_paused = true; }                              // Pause animation
    void resume() { m_paused = false; }                            // Resume animation
    bool isPaused() const { return m_paused; }                     // Is animation paused
    void setSpeed(float speed) { m_animationSpeed = speed; }       // Set playback speed
    float getSpeed() const { return m_animationSpeed; }            // Get playback speed

    // Debugging methods
    const Animation* getCurrentAnimation() const { return m_currentAnimation; } // Get current animation ref
    const sf::Sprite& getSprite() const { return m_sprite; }                    // Get sprite (const)
    sf::Sprite& getSprite() { return m_sprite; }                                // Get sprite

private:
    void* m_owner;                              // Owner of the component
    const Animation* m_currentAnimation = nullptr; // Current animation
    std::string m_currentAnimationName;         // Name of the current animation
    float m_time = 0.f;                         // Time accumulator
    int m_currentFrame = 0;                     // Current frame index
    int m_previousFrame = -1;                   // Previously displayed frame
    sf::Sprite m_sprite;                        // Sprite to render

    bool m_paused = false;                      // Is animation paused
    float m_animationSpeed = 1.0f;              // Speed multiplier

    std::map<int, FrameCallback> m_frameCallbacks;       // Frame-specific callbacks
    AnimationCallback m_animationCompleteCallback;       // Callback when anim completes

    // Helper methods
    void checkFrameCallbacks();                 // Check and execute frame callbacks
    void checkAnimationComplete();              // Check if animation is done

    bool needsHorizontalFlip() const;           // Should sprite be flipped horizontally
};
