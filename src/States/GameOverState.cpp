// ================================
// Game Over State - Defeat State with Atmospheric Smoke Effects and Encouragement System
// ================================
#include "States/GameOverState.h"
#include "States/PlayState.h"
#include "States/LevelsState.h"
#include "States/MenuState.h"
#include "States/StateMachine.h"
#include "Managers/ResourceManager.h"
#include "Managers/AudioManager.h"
#include "Core/Game.h"
#include <sstream>
#include <random>

// Constructor - Initialize game over state with defeat analysis
GameOverState::GameOverState(Game* game, StateMachine* machine, Reason reason,
    int finalScore, int wavesCompleted, float timeElapsed, int currentLevel)
    : m_game(game), m_machine(machine), m_reason(reason),
    m_finalScore(finalScore), m_wavesCompleted(wavesCompleted),
    m_timeElapsed(timeElapsed), m_currentLevel(currentLevel) {
    setupSmoke();    // Initialize atmospheric smoke particle system
    setupUI();       // Create defeat interface
}

// State lifecycle - play defeat sound
void GameOverState::onEnter() {
    AudioManager::getInstance().playSound("gameover", AudioManager::AudioCategory::UI ,0.3f);
}

// State lifecycle - no cleanup needed
void GameOverState::onExit() {}

// Handle button interactions
void GameOverState::handleEvent(const sf::Event& event) {
    for (auto& button : m_buttons)
        button->handleEvent(event);
}

// Update button animations and smoke particle system
void GameOverState::update(float dt) {
    // Update button animations
    for (auto& button : m_buttons)
        button->update(dt);

    // Update atmospheric smoke particles
    for (auto& smoke : m_smokeParticles) {
        // Move smoke upward
        smoke.position.y -= smoke.speed * dt;
        smoke.shape.setPosition(smoke.position);

        // Fade out smoke over time
        sf::Color c = smoke.shape.getFillColor();
        if (c.a > 2) {
            c.a -= static_cast<sf::Uint8>(dt * 20.f);
        }
        else {
            // Reset particle to bottom when fully faded
            smoke.position.y = GameConstants::WINDOW_HEIGHT;
            smoke.position.x = static_cast<float>(rand() % GameConstants::WINDOW_WIDTH);
            c.a = 120;  // Reset opacity
        }
        smoke.shape.setFillColor(c);
    }
}

// Render defeat screen with smoke effects
void GameOverState::render(sf::RenderWindow& window) {
    window.draw(m_backgroundSprite);    // Background
    window.draw(m_titleText);           // "GAME OVER" title
    window.draw(m_statsText);           // Performance statistics
    window.draw(m_tipText);             // Encouragement message

    // Render action buttons
    for (auto& button : m_buttons)
        button->draw(window);

    // Render atmospheric smoke particles
    for (const auto& smoke : m_smokeParticles)
        window.draw(smoke.shape);
}

// Create and layout defeat interface elements
void GameOverState::setupUI() {
    auto& rm = ResourceManager::getInstance();
    sf::Font& font = rm.getFont("bruce");
    sf::Texture& bgTexture = rm.getTexture("level_bg.png");
    sf::Texture& btnTexture = rm.getTexture("btn.png");

    // Setup semi-transparent background
    m_backgroundSprite.setTexture(bgTexture);
    m_backgroundSprite.setColor(sf::Color(255, 255, 255, 120));

    const float centerX = GameConstants::WINDOW_WIDTH / 2.f;
    float currentY = 200.f;

    // Large dramatic "GAME OVER" title
    m_titleText.setFont(font);
    m_titleText.setCharacterSize(72);
    m_titleText.setString("GAME OVER!");
    m_titleText.setFillColor(sf::Color(150, 30, 30));  // Dark red for drama
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setOrigin(titleBounds.width / 2.f, 0);
    m_titleText.setPosition(centerX, currentY);
    currentY += titleBounds.height + 40.f;

    // Performance statistics display
    std::ostringstream stats;
    stats << "Score: " << m_finalScore
        << "\nWaves: " << m_wavesCompleted
        << "\nTime: " << static_cast<int>(m_timeElapsed) << "s";

    m_statsText.setFont(font);
    m_statsText.setCharacterSize(28);
    m_statsText.setFillColor(sf::Color::Black);
    m_statsText.setString(stats.str());
    sf::FloatRect statsBounds = m_statsText.getLocalBounds();
    m_statsText.setOrigin(statsBounds.width / 2.f, 0);
    m_statsText.setPosition(centerX, currentY);
    currentY += statsBounds.height + 20.f;

    // Encouragement and tip text
    m_tipText.setFont(font);
    m_tipText.setCharacterSize(25);
    m_tipText.setFillColor(sf::Color(200, 60, 60));    // Motivational red
    m_tipText.setString(getEncouragementTip());
    sf::FloatRect tipBounds = m_tipText.getLocalBounds();
    m_tipText.setOrigin(tipBounds.width / 2.f, 0);
    m_tipText.setPosition(centerX, currentY);
    currentY += tipBounds.height + 50.f;

    // Setup action buttons
    sf::Vector2f buttonSize(240.f, 60.f);
    float spacingX = 30.f;

    // Define button actions with audio feedback
    struct ButtonDef {
        const char* label;
        std::function<void()> callback;
    };

    ButtonDef buttons[] = {
        { "Restart",      [this]() { onRestart(); } },      // Try again immediately
        { "Go Back",      [this]() { onBackToLevels(); } }, // Return to level selection
    };

    // Center buttons horizontally
    float totalWidth = std::size(buttons) * buttonSize.x + (std::size(buttons) - 1) * spacingX;
    float startX = centerX - totalWidth / 2.f;

    for (size_t i = 0; i < std::size(buttons); ++i) {
        auto& def = buttons[i];
        auto btn = std::make_unique<Button>(
            sf::Vector2f(startX + i * (buttonSize.x + spacingX), currentY),
            buttonSize, btnTexture, sf::IntRect(), font, def.label);

        // Add click sound and callback
        btn->setClickCallback([=]() {
            AudioManager::getInstance().playSound("click");
            def.callback();
            });

        m_buttons.push_back(std::move(btn));
    }
}

// Initialize atmospheric smoke particle system
void GameOverState::setupSmoke() {
    std::default_random_engine engine(static_cast<unsigned>(time(nullptr)));
    std::uniform_real_distribution<float> xDist(0.f, GameConstants::WINDOW_WIDTH);
    std::uniform_real_distribution<float> yDist(GameConstants::WINDOW_HEIGHT * 0.6f, GameConstants::WINDOW_HEIGHT);
    std::uniform_real_distribution<float> speedDist(15.f, 35.f);
    std::uniform_real_distribution<float> radiusDist(8.f, 15.f);

    m_smokeParticles.clear();

    // Create 60 smoke particles with randomized properties
    for (int i = 0; i < 60; ++i) {
        Smoke s;
        s.position = { xDist(engine), yDist(engine) };      // Random starting position
        s.speed = speedDist(engine);                        // Random rising speed

        float radius = radiusDist(engine);                  // Random particle size
        s.shape.setRadius(radius);
        s.shape.setOrigin(radius, radius);                  // Center origin
        s.shape.setFillColor(sf::Color(100, 100, 100, 120)); // Semi-transparent gray
        s.shape.setPosition(s.position);

        m_smokeParticles.push_back(s);
    }
}

// Button Action Handlers

// Restart current level immediately
void GameOverState::onRestart() {
    if (m_game && m_machine)
        m_machine->changeState(std::make_unique<PlayState>(m_game, m_machine, m_currentLevel));
}

// Return to level selection screen
void GameOverState::onBackToLevels() {
    if (m_game && m_machine)
        m_machine->changeState(std::make_unique<LevelsState>(m_game, m_machine));
}

// Generate encouraging message based on performance
std::string GameOverState::getEncouragementTip() const {
    // Could analyze m_reason, m_wavesCompleted, etc. for specific tips
    return "Failure is the key to success. Try again!";
}