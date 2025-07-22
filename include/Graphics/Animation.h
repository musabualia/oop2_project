// ================================
// include/Graphics/Animation.h
// ================================
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Animation {
public:
    // Constructor to initialize animation with texture and frame details
    Animation(const sf::Texture& texture, int frameWidth, int frameHeight, int frameCount, float duration);

    void update(float dt);                         // Update current frame based on time
    void applyToSprite(sf::Sprite& sprite) const;  // Apply current frame to sprite

    const sf::Texture& getTexture() const;         // Get texture used by the animation
    const sf::IntRect& getFrame(int index) const;  // Get specific frame rectangle
    int getFrameCount() const;                     // Get total number of frames
    float getDuration() const;                     // Get total animation duration

private:
    const sf::Texture& m_texture;                  // Texture containing animation frames
    std::vector<sf::IntRect> m_frames;             // List of frame rectangles
    int m_frameCount;                              // Total frame count
    float m_duration;                              // Duration for the full animation cycle
};
