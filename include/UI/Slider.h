#pragma once
#include <SFML/Graphics.hpp>
#include <functional>

class Slider {
public:
    Slider(const sf::Vector2f& position, const sf::Vector2f& size, float initialValue = 1.f);

    void setTexture(const sf::Texture& knobTexture);
    void setFont(const sf::Font& font);
    void setLabel(const std::string& text);

    void handleEvent(const sf::Event& event);
    void update(float dt);
    void render(sf::RenderWindow& window);

    float getValue() const;
    void setValue(float value);  
    void setKnobScale(float scale);


private:
    sf::RectangleShape m_track;
    sf::Sprite m_knob;
    sf::Text m_label;
    float m_value;
    bool m_dragging = false;
    float m_knobScale = 1.0f; 


    void updateKnobPosition();
};
