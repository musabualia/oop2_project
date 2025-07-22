#pragma once
#include "Core/Constants.h"
#include "Core/Timer.h"
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

class Button {
public:
    using ClickCallback = std::function<void()>;

    // Button style configuration
    struct ButtonStyle {
        sf::Color normalColor = sf::Color(60, 70, 80, 200);
        sf::Color hoverColor = sf::Color(80, 90, 100, 220);
        sf::Color pressedColor = sf::Color(40, 50, 60, 240);
        sf::Color disabledColor = sf::Color(40, 40, 40, 150);
        sf::Color textColor = sf::Color::White;
        sf::Color borderColor = sf::Color(100, 110, 120);
        float borderThickness = 2.0f;
        float cornerRadius = 8.0f;
        bool enableGlow = true;
        bool enableScale = true;
    };

    // Constructors
    Button(const sf::Vector2f& position, const sf::Vector2f& size, const std::string& text = "");
    Button(const sf::Vector2f& position, const sf::Vector2f& size,
        const sf::Texture& texture, const sf::IntRect& textureRect,
        const sf::Font& font, const std::string& text = "");

    // Core functionality
    void update(float dt);
    void draw(sf::RenderWindow& window) const;
    void handleEvent(const sf::Event& event);

    // Properties
    void setPosition(const sf::Vector2f& position);
    void setSize(const sf::Vector2f& size);
    void setText(const std::string& text);
    void setTexture(const sf::Texture& texture);
    void setFont(const sf::Font& font);
    void setEnabled(bool enabled);
    void setStyle(const ButtonStyle& style);

    // State queries
    bool isEnabled() const;
    bool isHovered() const;
    bool isPressed() const;
    sf::FloatRect getBounds() const;

    // Callback
    void setClickCallback(ClickCallback callback);
    void click(); // Programmatic trigger

    // Animation control
    void setAnimationSpeed(float speed);
    void setHoverScale(float scale);
    void setGlowIntensity(float intensity);

    // Static style presets
    static ButtonStyle createDefaultStyle();
    static ButtonStyle createVictoryStyle();
    static ButtonStyle createDefeatStyle();

private:
    // Core properties
    sf::Vector2f m_position;
    sf::Vector2f m_size;
    std::string m_text;
    bool m_enabled = true;
    bool m_useTexture = false;
    ButtonStyle m_style;

    // State management
    GameTypes::ButtonState m_state = GameTypes::ButtonState::Normal;
    GameTypes::ButtonState m_previousState = GameTypes::ButtonState::Normal;
    ClickCallback m_clickCallback;

    // Visual components
    sf::RectangleShape m_background;
    sf::Sprite m_sprite;
    sf::Text m_textObject;
    sf::Font m_defaultFont;

    // Animation properties
    float m_currentScale = 1.0f;
    float m_targetScale = 1.0f;
    float m_animationSpeed = 8.0f;
    float m_hoverScale = 1.05f;
    float m_pressScale = 0.95f;
    float m_glowIntensity = 1.0f;
    Timer m_hoverTimer;
    Timer m_pressTimer;

    // Glow effect
    sf::RectangleShape m_glowShape;
    float m_currentGlow = 0.0f;
    float m_targetGlow = 0.0f;

    // Text animation
    sf::Vector2f m_textOffset;
    float m_textScale = 1.0f;

    // Internal methods
    void updateState(const sf::Vector2f& mousePos, bool mousePressed);
    void updateAnimations(float dt);
    void updateScale(float dt);
    void updateGlow(float dt);
    void updateTextAnimation(float dt);
    void setupVisuals();
    void setupText();
    void applyStyle();
    sf::Color getCurrentBackgroundColor() const;
    sf::Color getCurrentTextColor() const;
    bool containsPoint(const sf::Vector2f& point) const;
    void playHoverSound();
    void playClickSound();

    // Rendering helpers
    void renderBackground(sf::RenderWindow& window) const;
    void renderGlow(sf::RenderWindow& window) const;
    void renderText(sf::RenderWindow& window) const;
    void renderTexture(sf::RenderWindow& window) const;
};