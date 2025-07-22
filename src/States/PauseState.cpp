// ================================
// Pause State - State Pattern Implementation for Game Pause Management
// ================================
#include "States/PauseState.h"
#include "Managers/ResourceManager.h"
#include "Managers/AudioManager.h"
#include "States/StateMachine.h"
#include "States/SettingsState.h"
#include "States/LevelsState.h"
#include "States/MenuState.h"
#include "Core/Game.h"
#include "Core/Constants.h"
#include <iostream>

// Constructor - Initialize pause state with semi-transparent overlay
PauseState::PauseState(Game* game, StateMachine* machine)
    : m_game(game), m_machine(machine) {
    setupUI();
}

// State lifecycle - no special setup needed on enter
void PauseState::onEnter() {}

// State lifecycle - no cleanup needed on exit
void PauseState::onExit() {}

// Handle input events with keyboard shortcuts and button interactions
void PauseState::handleEvent(const sf::Event& event) {
    // Keyboard shortcuts for quick navigation
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
            onResumeButton();                               // Quick resume with spacebar
        }
        else if (event.key.code == sf::Keyboard::Escape) {
            onMainMenuButton();                             // Quick exit with escape
        }
        else if (event.key.code == sf::Keyboard::Up) {
            updateButtonSelection(-1);                      // Navigate up
        }
        else if (event.key.code == sf::Keyboard::Down) {
            updateButtonSelection(1);                       // Navigate down
        }
        else if (event.key.code == sf::Keyboard::Enter && !m_buttons.empty()) {
            m_buttons[m_selectedButtonIndex]->click();      // Activate selected button
        }
    }

    // Forward mouse events to buttons for hover and click detection
    if (event.type == sf::Event::MouseMoved ||
        event.type == sf::Event::MouseButtonPressed ||
        event.type == sf::Event::MouseButtonReleased) {
        for (auto& btn : m_buttons)
            btn->handleEvent(event);
    }
}

// Update button animations and visual effects
void PauseState::update(float dt) {
    for (auto& btn : m_buttons)
        btn->update(dt);
}

// Render semi-transparent overlay and pause menu
void PauseState::render(sf::RenderWindow& window) {
    renderOverlay(window);      // Semi-transparent background
    renderMenu(window);         // Pause menu content
}

// Initialize UI elements and layout
void PauseState::setupUI() {
    // Create semi-transparent background overlay
    sf::Texture& bg = ResourceManager::getInstance().getTexture("level_bg.png");
    m_backgroundSprite.setTexture(bg);
    m_backgroundSprite.setColor(sf::Color(255, 255, 255, 120));  // Semi-transparent

    // Setup pause title text
    sf::Font& font = ResourceManager::getInstance().getFont("bruce");
    m_pauseTitle.setFont(font);
    m_pauseTitle.setString("PAUSED");
    m_pauseTitle.setCharacterSize(48);
    m_pauseTitle.setFillColor(sf::Color::White);

    // Center the title horizontally
    sf::FloatRect titleBounds = m_pauseTitle.getLocalBounds();
    m_pauseTitle.setOrigin(titleBounds.width / 2.f, titleBounds.height / 2.f);
    m_pauseTitle.setPosition(GameConstants::WINDOW_WIDTH / 2.f, 150.f);

    setupButtons();
}

// Create and configure pause menu buttons
void PauseState::setupButtons() {
    m_buttons.clear();

    // Load button assets
    const sf::Texture& btnTex = ResourceManager::getInstance().getTexture("btn.png");
    const sf::Font& font = ResourceManager::getInstance().getFont("bruce");

    // Button layout configuration
    sf::Vector2f center(GameConstants::WINDOW_WIDTH / 2.f, 240.f);
    sf::Vector2f size(250.f, 60.f);
    float spacing = 80.f;

    // Define button actions with audio feedback
    std::vector<std::pair<std::string, std::function<void()>>> actions = {
        {"Resume", [this]() {
            AudioManager::getInstance().playSound("click", AudioManager::AudioCategory::UI);
            onResumeButton();
        }},
        {"Settings", [this]() {
            AudioManager::getInstance().playSound("click", AudioManager::AudioCategory::UI);
            onSettingsButton();
        }},
        {"Main Menu", [this]() {
            AudioManager::getInstance().playSound("click", AudioManager::AudioCategory::UI);
            onMainMenuButton();
        }},
        {"Quit", [this]() {
            AudioManager::getInstance().playSound("click", AudioManager::AudioCategory::UI);
            onQuitButton();
        }}
    };

    // Create buttons with vertical layout
    for (size_t i = 0; i < actions.size(); ++i) {
        auto button = std::make_unique<Button>(
            sf::Vector2f(center.x - size.x / 2.f, center.y + i * spacing),
            size, btnTex, sf::IntRect(), font, actions[i].first
        );
        button->setClickCallback(actions[i].second);
        m_buttons.push_back(std::move(button));
    }

    m_selectedButtonIndex = 0;  // Start with first button selected
}

// Handle keyboard navigation between buttons
void PauseState::updateButtonSelection(int direction) {
    m_selectedButtonIndex += direction;
    // Wrap around at boundaries
    if (m_selectedButtonIndex < 0)
        m_selectedButtonIndex = static_cast<int>(m_buttons.size()) - 1;
    if (m_selectedButtonIndex >= static_cast<int>(m_buttons.size()))
        m_selectedButtonIndex = 0;
}

// Update button selection based on mouse hover
void PauseState::updateButtonHover(const sf::Vector2f& mousePos) {
    for (size_t i = 0; i < m_buttons.size(); ++i) {
        if (m_buttons[i]->getBounds().contains(mousePos)) {
            m_selectedButtonIndex = static_cast<int>(i);
            break;
        }
    }
}

// Render semi-transparent background overlay
void PauseState::renderOverlay(sf::RenderWindow& window) {
    window.draw(m_backgroundSprite);
}

// Render pause menu content
void PauseState::renderMenu(sf::RenderWindow& window) {
    window.draw(m_pauseTitle);
    for (auto& btn : m_buttons)
        btn->draw(window);
}

// Button Action Handlers

// Resume gameplay by popping pause state
void PauseState::onResumeButton() {
    if (m_machine) {
        m_machine->popState();  // Return to gameplay state
    }
}

// Open settings as overlay state
void PauseState::onSettingsButton() {
    if (m_game && m_machine) {
        m_machine->pushState(std::make_unique<SettingsState>(m_game, m_machine));
    }
}

// Return to main menu (abandoning current game)
void PauseState::onMainMenuButton() {
    if (m_game && m_machine) {
        m_machine->changeState(std::make_unique<MenuState>(m_game, m_machine));
    }
}

// Quit to level selection (abandoning current game)
void PauseState::onQuitButton() {
    if (m_game && m_machine) {
        m_machine->changeState(std::make_unique<LevelsState>(m_game, m_machine));
    }
}