#include "UI/Slider.h"
#include <algorithm>

Slider::Slider(const sf::Vector2f& position, const sf::Vector2f& size, float initialValue)
    : m_value(std::clamp(initialValue, 0.f, 1.f)) {
    m_track.setPosition(position);
    m_track.setSize(size);
    m_track.setFillColor(sf::Color(180, 180, 180));
}

void Slider::setTexture(const sf::Texture& knobTexture) {
    m_knob.setTexture(knobTexture);
    m_knob.setScale(m_knobScale, m_knobScale); 
    updateKnobPosition();
}


void Slider::setFont(const sf::Font& font) {
    m_label.setFont(font);
    m_label.setCharacterSize(18);
    m_label.setFillColor(sf::Color::White);
    m_label.setPosition(m_track.getPosition().x, m_track.getPosition().y - 30);
}

void Slider::setLabel(const std::string& text) {
    m_label.setString(text);
}

void Slider::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        if (m_knob.getGlobalBounds().contains((float)event.mouseButton.x, (float)event.mouseButton.y)) {
            m_dragging = true;
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased) {
        m_dragging = false;
    }
    else if (event.type == sf::Event::MouseMoved && m_dragging) {
        float x = static_cast<float>(event.mouseMove.x);
        float left = m_track.getPosition().x;
        float right = left + m_track.getSize().x;
        m_value = std::clamp((x - left) / m_track.getSize().x, 0.f, 1.f);
        updateKnobPosition();
    }
}

void Slider::update(float) {}

void Slider::render(sf::RenderWindow& window) {
    window.draw(m_track);
    window.draw(m_knob);
    window.draw(m_label);
}

float Slider::getValue() const {
    return m_value;
}

void Slider::setValue(float value) {
    m_value = std::clamp(value, 0.f, 1.f);
    updateKnobPosition();
}

void Slider::setKnobScale(float scale) {
    m_knobScale = scale;
    m_knob.setScale(m_knobScale, m_knobScale);
    updateKnobPosition();
}

void Slider::updateKnobPosition() {
    float knobX = m_track.getPosition().x + m_value * m_track.getSize().x - m_knob.getGlobalBounds().width / 2.f;
    float knobY = m_track.getPosition().y + m_track.getSize().y / 2.f - m_knob.getGlobalBounds().height / 2.f;
    m_knob.setPosition(knobX, knobY);
}
