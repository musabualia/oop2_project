#include "Managers/ResourceManager.h"
#include "Managers/AudioManager.h"
#include "States/LevelsState.h"
#include "States/MenuState.h"
#include "States/MoreState.h"
#include "States/SettingsState.h"
#include <iostream>

MenuState::MenuState(Game* game, StateMachine* machine)
    : m_game(game), m_machine(machine) {
    m_backgroundSprite.setTexture(ResourceManager::getInstance().getTexture("menu_background.png"));

    sf::Texture& btnTexture = ResourceManager::getInstance().getTexture("btn.png");
    sf::Font& font = ResourceManager::getInstance().getFont("bruce");
    sf::IntRect fullRect(0, 0, btnTexture.getSize().x, btnTexture.getSize().y);
    sf::Vector2f buttonSize(250.f, 60.f);

    float baseY = 350.f;
    float spacingY = 70.f;
    struct {
        const char* label;
        std::function<void()> callback;
    } buttonData[] = {
        { "Play", [this]() {
             AudioManager::getInstance().stopMusic(); 
             m_machine->changeState(std::make_unique<LevelsState>(m_game, m_machine));
        }},
        { "More", [this]() {
             AudioManager::getInstance().stopMusic(); 
             m_machine->pushState(std::make_unique<MoreState>(m_game, m_machine));
        }},
        { "Settings", [this]() {
             AudioManager::getInstance().stopMusic(); 
             m_machine->pushState(std::make_unique<SettingsState>(m_game, m_machine));
        }},
        { "Exit", [this]() {
             AudioManager::getInstance().stopMusic(); 
             m_game->quit();
        }}
    };


    for (int i = 0; i < 4; ++i) {
        auto btn = std::make_unique<Button>(
            sf::Vector2f(0.f, baseY + i * spacingY),
            buttonSize,
            btnTexture, fullRect,
            font, buttonData[i].label
        );
        btn->setClickCallback(buttonData[i].callback);
        m_buttons.push_back(std::move(btn));
    }

}

void MenuState::handleEvent(const sf::Event& event) {
    for (auto& button : m_buttons) {
        button->handleEvent(event);
    }
}

void MenuState::update(float dt) {
    for (auto& button : m_buttons) {
        button->update(dt);
    }
    m_titleAnimationTime += dt;
}

void MenuState::render(sf::RenderWindow& window) {
    // Stretch background to match window size
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u textureSize = m_backgroundSprite.getTexture()->getSize();

    m_backgroundSprite.setScale(
        static_cast<float>(windowSize.x) / textureSize.x,
        static_cast<float>(windowSize.y) / textureSize.y
    );

    window.draw(m_backgroundSprite);

    // Center buttons horizontally based on current window size
    for (auto& button : m_buttons) {
        sf::Vector2f size = button->getBounds().getSize();
        button->setPosition({ (windowSize.x - size.x) / 2.f, button->getBounds().top });
        button->draw(window);
    }
}

void MenuState::onEnter() {
    AudioManager::getInstance().playMusic("Menu", true);
}

void MenuState::onExit() {
    AudioManager::getInstance().stopMusic();
}