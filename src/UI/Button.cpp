// ================================
// src/UI/Button.cpp 
// ================================
#include "UI/Button.h"
#include "Managers/AudioManager.h"
#include "Managers/ResourceManager.h"
#include <algorithm>
#include <cmath>

Button::Button(const sf::Vector2f& position, const sf::Vector2f& size, const std::string& text)
    : m_position(position), m_size(size), m_text(text), m_style(createDefaultStyle())
    , m_hoverTimer(0.3f), m_pressTimer(0.15f)
{
    setupVisuals();
    setupText();
}

Button::Button(const sf::Vector2f& position, const sf::Vector2f& size,
    const sf::Texture& texture, const sf::IntRect& textureRect,
    const sf::Font& font, const std::string& text)
    : m_position(position), m_size(size), m_text(text), m_useTexture(true)
    , m_style(createDefaultStyle()), m_hoverTimer(0.3f), m_pressTimer(0.15f)
{
    m_sprite.setTexture(texture);
    if (textureRect != sf::IntRect()) {
        m_sprite.setTextureRect(textureRect);
    }
    m_textObject.setFont(font);
    setupVisuals();
    setupText();
}

void Button::update(float dt) {
    updateAnimations(dt);

    // Update timers
    m_hoverTimer.update(dt);
    m_pressTimer.update(dt);
}

void Button::draw(sf::RenderWindow& window) const {
    if (m_style.enableGlow && m_currentGlow > 0) {
        renderGlow(window);
    }

    if (m_useTexture) {
        renderTexture(window);
    }
    else {
        renderBackground(window);
    }

    renderText(window);
}

void Button::handleEvent(const sf::Event& event) {
    if (!m_enabled) return;

    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
        updateState(mousePos, false);
    }
    else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            updateState(mousePos, true);
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            if (m_state == GameTypes::ButtonState::Pressed && containsPoint(mousePos)) {
                playClickSound();
                click();
            }
            updateState(mousePos, false);
        }
    }
}

void Button::setPosition(const sf::Vector2f& position) {
    m_position = position;
    setupVisuals();
    setupText();
}

void Button::setSize(const sf::Vector2f& size) {
    m_size = size;
    setupVisuals();
}

void Button::setText(const std::string& text) {
    m_text = text;
    m_textObject.setString(text);
    setupText();
}

void Button::setTexture(const sf::Texture& texture) {
    m_sprite.setTexture(texture);
    m_useTexture = true;
}

void Button::setFont(const sf::Font& font) {
    m_textObject.setFont(font);
    setupText();
}

void Button::setEnabled(bool enabled) {
    m_enabled = enabled;
    if (!enabled) {
        m_state = GameTypes::ButtonState::Disabled;
        m_targetScale = 1.0f;
        m_targetGlow = 0.0f;
    }
}

void Button::setStyle(const ButtonStyle& style) {
    m_style = style;
    applyStyle();
}

bool Button::isEnabled() const {
    return m_enabled;
}

bool Button::isHovered() const {
    return m_state == GameTypes::ButtonState::Hovered;
}

bool Button::isPressed() const {
    return m_state == GameTypes::ButtonState::Pressed;
}

sf::FloatRect Button::getBounds() const {
    return sf::FloatRect(m_position, m_size);
}

void Button::setClickCallback(ClickCallback callback) {
    m_clickCallback = callback;
}

void Button::click() {
    if (m_enabled && m_clickCallback) {
        m_clickCallback();
    }
}

void Button::setAnimationSpeed(float speed) {
    m_animationSpeed = speed;
}

void Button::setHoverScale(float scale) {
    m_hoverScale = scale;
}

void Button::setGlowIntensity(float intensity) {
    m_glowIntensity = intensity;
}

Button::ButtonStyle Button::createDefaultStyle() {
    ButtonStyle style;
    style.normalColor = sf::Color(60, 70, 80, 200);
    style.hoverColor = sf::Color(80, 90, 100, 220);
    style.pressedColor = sf::Color(40, 50, 60, 240);
    style.disabledColor = sf::Color(40, 40, 40, 150);
    style.textColor = sf::Color::Black;
    style.borderColor = sf::Color(100, 110, 120);
    style.borderThickness = 2.0f;
    style.enableGlow = true;
    style.enableScale = true;
    return style;
}

Button::ButtonStyle Button::createVictoryStyle() {
    ButtonStyle style = createDefaultStyle();
    style.normalColor = sf::Color(80, 120, 60, 200);
    style.hoverColor = sf::Color(100, 140, 80, 220);
    style.pressedColor = sf::Color(60, 100, 40, 240);
    style.borderColor = sf::Color(120, 160, 100);
    style.textColor = sf::Color(255, 255, 200);
    return style;
}

Button::ButtonStyle Button::createDefeatStyle() {
    ButtonStyle style = createDefaultStyle();
    style.normalColor = sf::Color(120, 60, 60, 200);
    style.hoverColor = sf::Color(140, 80, 80, 220);
    style.pressedColor = sf::Color(100, 40, 40, 240);
    style.borderColor = sf::Color(160, 100, 100);
    style.textColor = sf::Color(255, 200, 200);
    return style;
}

void Button::updateState(const sf::Vector2f& mousePos, bool mousePressed) {
    if (!m_enabled) {
        m_state = GameTypes::ButtonState::Disabled;
        return;
    }

    GameTypes::ButtonState newState = GameTypes::ButtonState::Normal;

    if (containsPoint(mousePos)) {
        if (mousePressed) {
            newState = GameTypes::ButtonState::Pressed;
        }
        else {
            newState = GameTypes::ButtonState::Hovered;
        }
    }

    // Handle state transitions
    if (newState != m_state) {
        m_previousState = m_state;
        m_state = newState;

        // Update target values based on new state
        switch (m_state) {
        case GameTypes::ButtonState::Normal:
            m_targetScale = 1.0f;
            m_targetGlow = 0.0f;
            break;
        case GameTypes::ButtonState::Hovered:
            if (m_previousState == GameTypes::ButtonState::Normal) {
                playHoverSound();
            }
            m_targetScale = m_style.enableScale ? m_hoverScale : 1.0f;
            m_targetGlow = m_style.enableGlow ? 0.3f : 0.0f;
            m_hoverTimer.restart();
            break;
        case GameTypes::ButtonState::Pressed:
            m_targetScale = m_style.enableScale ? m_pressScale : 1.0f;
            m_targetGlow = m_style.enableGlow ? 0.6f : 0.0f;
            m_pressTimer.restart();
            break;
        case GameTypes::ButtonState::Disabled:
            m_targetScale = 1.0f;
            m_targetGlow = 0.0f;
            break;
        }
    }
}

void Button::updateAnimations(float dt) {
    updateScale(dt);
    updateGlow(dt);
    updateTextAnimation(dt);
}

void Button::updateScale(float dt) {
    if (std::abs(m_currentScale - m_targetScale) > 0.001f) {
        float diff = m_targetScale - m_currentScale;
        m_currentScale += diff * m_animationSpeed * dt;

        // Clamp to target if very close
        if (std::abs(diff) < 0.001f) {
            m_currentScale = m_targetScale;
        }
    }
}

void Button::updateGlow(float dt) {
    if (std::abs(m_currentGlow - m_targetGlow) > 0.001f) {
        float diff = m_targetGlow - m_currentGlow;
        m_currentGlow += diff * m_animationSpeed * dt;

        // Clamp to target if very close
        if (std::abs(diff) < 0.001f) {
            m_currentGlow = m_targetGlow;
        }
    }
}

void Button::updateTextAnimation(float dt) {
    // Subtle text animation based on button state
    switch (m_state) {
    case GameTypes::ButtonState::Pressed:
        m_textOffset = sf::Vector2f(1, 1); // Slight press effect
        m_textScale = 0.98f;
        break;
    case GameTypes::ButtonState::Hovered:
    {
        // Gentle pulsing effect - use braces to create scope for variable
        float pulse = 1.0f + 0.02f * std::sin(m_hoverTimer.getElapsed() * 8.0f);
        m_textScale = pulse;
        m_textOffset = sf::Vector2f(0, -1); // Slight lift
        break;
    }
    default:
        m_textOffset = sf::Vector2f(0, 0);
        m_textScale = 1.0f;
        break;
    }
}

void Button::setupVisuals() {
    // Setup background shape
    m_background.setSize(m_size);
    m_background.setPosition(m_position);

    // Setup glow shape (slightly larger)
    float glowExpansion = 10.0f;
    m_glowShape.setSize(m_size + sf::Vector2f(glowExpansion * 2, glowExpansion * 2));
    m_glowShape.setPosition(m_position - sf::Vector2f(glowExpansion, glowExpansion));

    // Setup sprite if using texture
    if (m_useTexture) {
        m_sprite.setPosition(m_position);

        // Scale sprite to fit button size
        sf::IntRect textureRect = m_sprite.getTextureRect();
        if (textureRect.width > 0 && textureRect.height > 0) {
            float scaleX = m_size.x / textureRect.width;
            float scaleY = m_size.y / textureRect.height;
            m_sprite.setScale(scaleX, scaleY);
        }
    }

    applyStyle();
}

void Button::setupText() {
        if (m_text.empty()) return;

        m_textObject.setString(m_text);
        m_textObject.setCharacterSize(28); // Adjust as needed
        if (m_text == "Main Menu")
            m_textObject.setCharacterSize(20);
        m_textObject.setFillColor(m_style.textColor); // Style color or fixed one

        // Correctly center the text by setting origin
        sf::FloatRect bounds = m_textObject.getLocalBounds();
        m_textObject.setOrigin(bounds.left + bounds.width / 2.f,
            bounds.top + bounds.height / 2.f);

        // Place at the center of the button (not sprite)
        m_textObject.setPosition(m_position.x + m_size.x / 2.f,
            m_position.y + m_size.y / 2.f);
    }


void Button::applyStyle() {
    m_background.setFillColor(getCurrentBackgroundColor());
    m_background.setOutlineThickness(m_style.borderThickness);
    m_background.setOutlineColor(m_style.borderColor);
    m_textObject.setFillColor(getCurrentTextColor());
}

sf::Color Button::getCurrentBackgroundColor() const {
    switch (m_state) {
    case GameTypes::ButtonState::Hovered:
        return m_style.hoverColor;
    case GameTypes::ButtonState::Pressed:
        return m_style.pressedColor;
    case GameTypes::ButtonState::Disabled:
        return m_style.disabledColor;
    default:
        return m_style.normalColor;
    }
}

sf::Color Button::getCurrentTextColor() const {
    if (m_state == GameTypes::ButtonState::Disabled) {
        sf::Color dimmed = m_style.textColor;
        dimmed.a = 128; // 50% opacity
        return dimmed;
    }
    return m_style.textColor;
}

bool Button::containsPoint(const sf::Vector2f& point) const {
    return getBounds().contains(point);
}

void Button::playHoverSound() {
    try {
        AudioManager::getInstance().playSound("button_hover", AudioManager::AudioCategory::UI, 0.3f);
    }
    catch (...) {
        // Sound not available, continue silently
    }
}

void Button::playClickSound() {
    try {
        AudioManager::getInstance().playSound("click", AudioManager::AudioCategory::UI, 0.5f);
    }
    catch (...) {
        // Sound not available, continue silently
    }
}

void Button::renderBackground(sf::RenderWindow& window) const {
    // Apply scale transformation
    sf::Transform transform;
    sf::Vector2f center = m_position + m_size * 0.5f;
    transform.translate(center);
    transform.scale(m_currentScale, m_currentScale);
    transform.translate(-center);

    window.draw(m_background, transform);
}

void Button::renderGlow(sf::RenderWindow& window) const {
    if (m_currentGlow <= 0) return;

    sf::Color glowColor = m_style.hoverColor;
    glowColor.a = static_cast<sf::Uint8>(50 * m_currentGlow * m_glowIntensity);

    sf::RectangleShape glow = m_glowShape;
    glow.setFillColor(glowColor);
    glow.setOutlineThickness(0);

    // Apply scale transformation
    sf::Transform transform;
    sf::Vector2f center = m_position + m_size * 0.5f;
    transform.translate(center);
    transform.scale(m_currentScale, m_currentScale);
    transform.translate(-center);

    window.draw(glow, transform);
}

void Button::renderText(sf::RenderWindow& window) const {
    if (m_text.empty()) return;

    sf::Text textCopy = m_textObject;
    textCopy.setFillColor(getCurrentTextColor());

    textCopy.setPosition(m_textObject.getPosition() + m_textOffset);

    // Scale around the text origin (which is centered)
    sf::Transform transform;
    transform.translate(textCopy.getPosition());
    transform.scale(m_currentScale * m_textScale, m_currentScale * m_textScale);
    transform.translate(-textCopy.getPosition());

    window.draw(textCopy, transform);
}


void Button::renderTexture(sf::RenderWindow& window) const {
    if (!m_useTexture) return;

    sf::Sprite spriteCopy = m_sprite;

    // Apply tint based on state
    sf::Color tint = sf::Color::White;
    switch (m_state) {
    case GameTypes::ButtonState::Hovered:
        tint = sf::Color(220, 220, 255); // Slight blue tint
        break;
    case GameTypes::ButtonState::Pressed:
        tint = sf::Color(180, 180, 180); // Darker
        break;
    case GameTypes::ButtonState::Disabled:
        tint = sf::Color(128, 128, 128); // Gray
        break;
    default:
        break;
    }
    spriteCopy.setColor(tint);

    // Apply scale transformation
    sf::Transform transform;
    sf::Vector2f center = m_position + m_size * 0.5f;
    transform.translate(center);
    transform.scale(m_currentScale, m_currentScale);
    transform.translate(-center);

    window.draw(spriteCopy, transform);
}