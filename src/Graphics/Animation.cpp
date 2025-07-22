// ================================
// src/Graphics/Animation.cpp
// ================================
#include "Graphics/Animation.h"

Animation::Animation(const sf::Texture& texture, int frameWidth, int frameHeight, int frameCount, float duration)
    : m_texture(texture), m_frameCount(frameCount), m_duration(duration) {

    int textureWidth = texture.getSize().x;
    int textureHeight = texture.getSize().y;
    int framesPerRow = textureWidth / frameWidth;
    int rows = (frameCount + framesPerRow - 1) / framesPerRow;

    for (int i = 0; i < frameCount; ++i) {
        int row = i / framesPerRow;
        int col = i % framesPerRow;

        sf::IntRect frameRect(
            col * frameWidth,
            row * frameHeight,
            frameWidth,
            frameHeight
        );

        m_frames.push_back(frameRect);
    }
}

void Animation::update(float dt) {
}

void Animation::applyToSprite(sf::Sprite& sprite) const {
    sprite.setTexture(m_texture);
    sprite.setTextureRect(m_frames[0]);
}

const sf::Texture& Animation::getTexture() const {
    return m_texture;
}

const sf::IntRect& Animation::getFrame(int index) const {
    return m_frames.at(index);
}

int Animation::getFrameCount() const {
    return m_frameCount;
}

float Animation::getDuration() const {
    return m_duration;
}