// ================================
// include/Physics/PhysicsUtils.h
// ================================
#pragma once
#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>

namespace PhysicsUtils {
    constexpr float PIXELS_PER_METER = 30.0f;

    inline b2Vec2 sfmlToBox2D(const sf::Vector2f& vector) {
        return b2Vec2(vector.x / PIXELS_PER_METER, vector.y / PIXELS_PER_METER);
    }

    inline sf::Vector2f box2DToSFML(const b2Vec2& vector) {
        return sf::Vector2f(vector.x * PIXELS_PER_METER, vector.y * PIXELS_PER_METER);
    }

    inline float sfmlToBox2D(float pixels) {
        return pixels / PIXELS_PER_METER;
    }

    inline float box2DToSFML(float meters) {
        return meters * PIXELS_PER_METER;
    }
}