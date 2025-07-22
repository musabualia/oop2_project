// ================================
// LevelsState.cpp - Scrollable Level Selection Implementation
// ================================
#include "States/LevelsState.h"
#include "States/MenuState.h"
#include "Managers/ResourceManager.h"
#include "Managers/AudioManager.h"
#include "Core/Game.h"
#include "Core/Constants.h"
#include "States/PlayState.h"
#include "UI/Button.h"
#include <iostream>

// Constructor - Initialize scrollable level selection interface
LevelsState::LevelsState(Game* game, StateMachine* machine)
    : m_game(game), m_machine(machine) {
    loadBackgroundImages();  // Load panoramic background
    setupUI();              // Initialize UI elements
    setupMapView();         // Setup scrollable camera
}

// Initialize complete UI system
void LevelsState::setupUI() {
    generateLevelPositions(); // Calculate level button positions
    setupLevelButtons();      // Create interactive level buttons
    setupWarningTape();       // Add decorative elements
    setupBackToMenuButton();  // Add navigation button
}

// Setup back to menu navigation button
void LevelsState::setupBackToMenuButton() {
    auto& rm = ResourceManager::getInstance();
    m_backToMenuTexture = rm.getTexture("back_to_menu.png");

    m_backToMenuSprite.setTexture(m_backToMenuTexture);
    m_backToMenuSprite.setScale(0.40f, 0.30f);

    // Position in top right corner with proper scaling
    sf::Vector2u windowSize = m_game->getWindow().getSize();
    float spriteWidth = m_backToMenuTexture.getSize().x * 0.40f;
    m_backToMenuSprite.setPosition(
        windowSize.x - spriteWidth - 20.f,  // 20px margin from right edge
        60.f                                // 60px from top
    );

    m_backToMenuBounds = m_backToMenuSprite.getGlobalBounds();
}

// Setup decorative warning tape elements
void LevelsState::setupWarningTape() {
    m_warningTapeTexture = ResourceManager::getInstance().getTexture("warningTape.png");

    // Scale tape to span entire map width
    float scaleX = m_totalMapWidth / static_cast<float>(m_warningTapeTexture.getSize().x);
    float scaleY = 1.0f;

    // Top warning tape
    m_warningTapeTop.setTexture(m_warningTapeTexture);
    m_warningTapeTop.setScale(scaleX, scaleY);
    m_warningTapeTop.setPosition(0.f, 0.f);

    // Bottom warning tape
    float bottomY = static_cast<float>(m_game->getWindow().getSize().y - m_warningTapeTexture.getSize().y * scaleY);
    m_warningTapeBottom.setTexture(m_warningTapeTexture);
    m_warningTapeBottom.setScale(scaleX, scaleY);
    m_warningTapeBottom.setPosition(0.f, bottomY);
}

// Load and setup three-part panoramic background
void LevelsState::loadBackgroundImages() {
    auto& rm = ResourceManager::getInstance();
    m_leftTexture = rm.getTexture("levelbackground_left.png");
    m_middleTexture = rm.getTexture("levelbackground_middle.png");
    m_rightTexture = rm.getTexture("levelbackground_right.png");

    // Calculate scaling to fit window height
    sf::Vector2u windowSize = m_game->getWindow().getSize();
    float scaleY = static_cast<float>(windowSize.y) / m_leftTexture.getSize().y;
    float scaledWidth = m_leftTexture.getSize().x * scaleY;

    // Position three background sections seamlessly
    m_leftBackground.setTexture(m_leftTexture);
    m_leftBackground.setPosition(0, 0);
    m_leftBackground.setScale(scaleY, scaleY);

    m_middleBackground.setTexture(m_middleTexture);
    m_middleBackground.setPosition(scaledWidth, 0);
    m_middleBackground.setScale(scaleY, scaleY);

    m_rightBackground.setTexture(m_rightTexture);
    m_rightBackground.setPosition(scaledWidth * 2.0f, 0);
    m_rightBackground.setScale(scaleY, scaleY);

    m_totalMapWidth = scaledWidth * 3.0f;  // Total scrollable width
}

// Generate positions for level buttons along the map
void LevelsState::generateLevelPositions() {
    m_levelPositions.clear();
    float scale = static_cast<float>(m_game->getWindow().getSize().y) / m_leftTexture.getSize().y;

    // Predefined positions for 17 levels across the panoramic map
    std::vector<sf::Vector2f> raw = {
        {100, 265}, {160, 260}, {225, 260}, {290, 260}, {355, 262}, {420, 250},
        {485, 255}, {550, 260}, {615, 268}, {680, 270}, {745, 270}, {810, 268},
        {875, 268}, {940, 268}, {1005, 265}, {1070, 263}, {1135, 265}
    };

    const float yOffset = -20.0f;  // Vertical adjustment

    // Scale and apply offset to each position
    for (const auto& pos : raw)
        m_levelPositions.emplace_back(pos.x * scale, (pos.y + yOffset) * scale);
}

void LevelsState::setupLevelButtons() {
    m_levelButtons.clear();
    const auto& font = ResourceManager::getInstance().getFont("bruce");
    const auto& texture = ResourceManager::getInstance().getTexture("level_btn.png");
    const sf::IntRect fullRect(0, 0, texture.getSize().x, texture.getSize().y);

    const int iconSize = GameConstants::LEVEL_ICON_SIZE;
    const float iconScale = GameConstants::LEVEL_ICON_SCALE * 5.0f;
    const sf::Vector2f buttonSize(iconSize * iconScale, iconSize * iconScale); // square

    for (size_t i = 0; i < m_levelPositions.size(); ++i) {
        sf::Vector2f pos = m_levelPositions[i];
        pos.y -= 20.0f;

        auto button = std::make_unique<Button>(
            pos,
            buttonSize,
            texture,
            fullRect,
            font,
            std::to_string(i + 1)
        );

        int levelIndex = static_cast<int>(i);
        button->setClickCallback([this, levelIndex]() {
            onLevelSelected(levelIndex);
            });

        m_levelButtons.push_back(std::move(button));
    }
}

void LevelsState::setupMapView() {
    auto size = m_game->getWindow().getSize();
    m_mapView.setSize((float)size.x, (float)size.y);
    m_mapView.setCenter((float)size.x / 2, (float)size.y / 2);
    m_maxScrollRight = std::max(0.0f, m_totalMapWidth - size.x);
}

void LevelsState::handleEvent(const sf::Event& event) {
    // Handle back to menu button click instead of ESC
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {

        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

        // Check if clicked on back to menu button
        if (m_backToMenuBounds.contains(mousePos)) {
            m_machine->changeState(std::make_unique<MenuState>(m_game, m_machine));
            return;
        }
    }

    updateScrolling(event);

    sf::Vector2f worldMouse = m_game->getWindow().mapPixelToCoords(
        sf::Mouse::getPosition(m_game->getWindow()), m_mapView);

    sf::View originalView = m_game->getWindow().getView();
    m_game->getWindow().setView(m_mapView);

    // Forward adjusted events to each button
    for (auto& button : m_levelButtons) {
        sf::Event adjusted = event;

        if (event.type == sf::Event::MouseMoved) {
            adjusted.mouseMove.x = static_cast<int>(worldMouse.x);
            adjusted.mouseMove.y = static_cast<int>(worldMouse.y);
        }
        else if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
            adjusted.mouseButton.x = static_cast<int>(worldMouse.x);
            adjusted.mouseButton.y = static_cast<int>(worldMouse.y);
        }

        button->handleEvent(adjusted);
    }

    // Restore original view
    m_game->getWindow().setView(originalView);
}



void LevelsState::updateScrolling(const sf::Event& event) {
    auto current = (sf::Vector2f)sf::Mouse::getPosition(m_game->getWindow());

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        m_isDragging = true;
        m_lastMousePos = current;
    }
    else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        m_isDragging = false;
    }
    else if (event.type == sf::Event::MouseMoved && m_isDragging) {
        m_mapScrollX += (m_lastMousePos.x - current.x) * m_scrollSpeed;
        clampScrollPosition();
        updateMapView();
        m_lastMousePos = current;
    }
    else if (event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
        m_mapScrollX -= event.mouseWheelScroll.delta * 100.f;
        clampScrollPosition();
        updateMapView();
    }
}

void LevelsState::clampScrollPosition() {
    m_mapScrollX = std::clamp(m_mapScrollX, m_maxScrollLeft, m_maxScrollRight);
}

void LevelsState::updateMapView() {
    auto winSize = m_game->getWindow().getSize();
    m_mapView.setCenter(winSize.x / 2.0f + m_mapScrollX, winSize.y / 2.0f);
}

void LevelsState::update(float dt) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        m_mapScrollX -= 300.0f * dt;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        m_mapScrollX += 300.0f * dt;
    }
    clampScrollPosition();
    updateMapView();
}

void LevelsState::render(sf::RenderWindow& window) {
    window.setView(m_mapView);
    window.draw(m_leftBackground);
    window.draw(m_middleBackground);
    window.draw(m_rightBackground);
    window.draw(m_warningTapeTop);
    window.draw(m_warningTapeBottom);

    for (auto& button : m_levelButtons)
        button->draw(window);

    window.setView(window.getDefaultView());
    window.draw(m_backToMenuSprite);

    sf::Text hint("Drag or use arrow keys to scroll",
        ResourceManager::getInstance().getFont("bruce"));
    hint.setCharacterSize(20);
    hint.setFillColor(sf::Color::White);
    hint.setPosition(10, 10);
    window.draw(hint);
}


void LevelsState::onEnter() {
    AudioManager::getInstance().playMusic("Level", true);
}

void LevelsState::onExit() {
    AudioManager::getInstance().stopMusic();
}

void LevelsState::onLevelSelected(int levelIndex) {
    AudioManager::getInstance().playSound("click", AudioManager::AudioCategory::UI);

    int selectedLevel = levelIndex + 1;
    m_machine->changeState(std::make_unique<PlayState>(m_game, m_machine, selectedLevel));
}