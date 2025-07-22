// ================================
// Victory State - Celebration State with Physics-Based Confetti and Score Analysis
// ================================
#include "States/VictoryState.h"
#include "States/MenuState.h"
#include "States/LevelsState.h"
#include "States/PlayState.h"
#include "States/StateMachine.h"
#include "Managers/ResourceManager.h"
#include "Managers/AudioManager.h"
#include "Core/Game.h"
#include <sstream>
#include <random>

// Constructor - Initialize victory state with performance statistics
VictoryState::VictoryState(Game* game, StateMachine* machine,
    int finalScore, int wavesCompleted, float timeElapsed,
    int currentLevel, bool perfectVictory)
    : m_game(game), m_machine(machine),
    m_finalScore(finalScore), m_wavesCompleted(wavesCompleted),
    m_timeElapsed(timeElapsed), m_currentLevel(currentLevel),
    m_perfectVictory(perfectVictory) {

    calculateScores();          // Calculate bonus points and totals
    checkForNewHighScore();     // Check if this is a new record
    setupBox2D();              // Initialize confetti physics system
    setupUI();                 // Create victory interface
}

// State lifecycle - play victory sound and effects
void VictoryState::onEnter() {
    AudioManager::getInstance().playSound("level_up", AudioManager::AudioCategory::UI);
}

// State lifecycle - no cleanup needed
void VictoryState::onExit() {}

// Handle button interactions
void VictoryState::handleEvent(const sf::Event& event) {
    for (auto& button : m_buttons)
        button->handleEvent(event);
}

// Update button animations and physics simulation
void VictoryState::update(float dt) {
    // Update button animations
    for (auto& button : m_buttons)
        button->update(dt);

    // Step physics simulation for confetti
    if (m_world)
        m_world->Step(dt, 6, 2);
}

// Render victory screen with animated confetti
void VictoryState::render(sf::RenderWindow& window) {
    window.draw(m_backgroundSprite);    // Background
    window.draw(m_statsText);           // Score statistics
    window.draw(m_tipText);             // Achievement messages

    // Render action buttons
    for (auto& button : m_buttons)
        button->draw(window);

    // Render physics-based confetti particles
    for (auto* body : m_confetti) {
        b2Vec2 pos = body->GetPosition();
        sf::CircleShape confetti(5.f);
        confetti.setFillColor(sf::Color(rand() % 255, rand() % 255, rand() % 255));
        confetti.setPosition(pos.x * 30.f, pos.y * 30.f);  // Convert physics to screen coordinates
        window.draw(confetti);
    }
}

// Create and layout victory interface elements
void VictoryState::setupUI() {
    auto& rm = ResourceManager::getInstance();
    sf::Font& font = rm.getFont("bruce");
    sf::Texture& bgTexture = rm.getTexture("level_bg.png");
    sf::Texture& btnTexture = rm.getTexture("btn.png");

    // Setup semi-transparent background
    m_backgroundSprite.setTexture(bgTexture);
    m_backgroundSprite.setColor(sf::Color(255, 255, 255, 150));

    const float centerX = GameConstants::WINDOW_WIDTH / 2.f;
    float currentY = 200.f;

    // Create comprehensive statistics display
    std::ostringstream stats;
    stats << "SCORE: " << m_finalScore << "\nWAVES: " << m_wavesCompleted
        << "\nTIME: " << static_cast<int>(m_timeElapsed) << "s"
        << "\nBONUS: " << m_bonusScore << "\nTOTAL: " << m_totalScore;

    m_statsText.setFont(font);
    m_statsText.setCharacterSize(32);
    m_statsText.setFillColor(sf::Color(0, 0, 0));
    m_statsText.setString(stats.str());
    sf::FloatRect statsBounds = m_statsText.getLocalBounds();
    m_statsText.setOrigin(statsBounds.width / 2.f, 0);
    m_statsText.setPosition(centerX, currentY);
    currentY += statsBounds.height + 30.f;

    // Create achievement and encouragement text
    std::string tip;
    if (m_newHighScore) tip += " NEW HIGH SCORE!\n";
    if (m_perfectVictory) tip += " PERFECT VICTORY!";

    m_tipText.setFont(font);
    m_tipText.setCharacterSize(22);
    m_tipText.setFillColor(sf::Color(20, 180, 120));  // Green color for positive feedback
    m_tipText.setString(tip);
    sf::FloatRect tipBounds = m_tipText.getLocalBounds();
    m_tipText.setOrigin(tipBounds.width / 2.f, 0);
    m_tipText.setPosition(centerX, currentY);
    currentY += tipBounds.height + 50.f;

    // Setup action buttons with clean layout
    sf::Vector2f buttonSize(200.f, 60.f);
    float spacingX = 40.f;

    // Define button actions with audio feedback
    struct ButtonDef {
        const char* label;
        std::function<void()> callback;
    };

    ButtonDef buttons[] = {
        { "Continue",  [this]() { onContinue(); } },    // Proceed to next level
        { "Replay",    [this]() { onReplay(); } },      // Retry current level
    };

    // Center buttons horizontally in a single row
    float totalWidth = std::size(buttons) * buttonSize.x + (std::size(buttons) - 1) * spacingX;
    float startX = centerX - totalWidth / 2.f;
    float buttonY = currentY;

    for (size_t i = 0; i < std::size(buttons); ++i) {
        const auto& def = buttons[i];
        sf::Vector2f pos(startX + i * (buttonSize.x + spacingX), buttonY);

        auto btn = std::make_unique<Button>(
            pos, buttonSize,
            btnTexture, sf::IntRect(),
            font, def.label
        );

        // Add click sound and callback
        btn->setClickCallback([=]() {
            AudioManager::getInstance().playSound("click");
            def.callback();
            });

        m_buttons.push_back(std::move(btn));
    }
}

// Initialize Box2D physics world for confetti celebration effect
void VictoryState::setupBox2D() {
    b2Vec2 gravity(0.f, 9.8f);  // Downward gravity
    m_world = std::make_unique<b2World>(gravity);

    // Create 80 confetti particles with random initial positions
    for (int i = 0; i < 80; ++i) {
        b2BodyDef bd;
        bd.type = b2_dynamicBody;

        // Random horizontal position across screen width
        float x = static_cast<float>(rand() % GameConstants::WINDOW_WIDTH) / 30.f;
        // Start above screen with random height variation
        float y = -static_cast<float>(rand() % 40);
        bd.position.Set(x, y);

        b2Body* body = m_world->CreateBody(&bd);

        // Create small circular confetti pieces
        b2CircleShape shape;
        shape.m_radius = 0.2f;

        b2FixtureDef fd;
        fd.shape = &shape;
        fd.density = 1.f;
        fd.friction = 0.3f;

        body->CreateFixture(&fd);
        m_confetti.push_back(body);
    }
}

// Calculate performance-based bonus scoring
void VictoryState::calculateScores() {
    // Time bonus: faster completion = higher bonus
    m_bonusScore = (300 - static_cast<int>(m_timeElapsed)) + (m_wavesCompleted * 50);

    // Perfect victory bonus
    if (m_perfectVictory) m_bonusScore += 500;

    // Calculate final total
    m_totalScore = m_finalScore + m_bonusScore;
}

// Check if player achieved a new high score
void VictoryState::checkForNewHighScore() {
    // Simplified high score check (could integrate with HighScoreManager)
    m_newHighScore = (m_totalScore > 1000);
}

// Button Action Handlers

// Continue to level selection (progression)
void VictoryState::onContinue() {
    if (m_game && m_machine)
        m_machine->changeState(std::make_unique<LevelsState>(m_game, m_machine));
}

// Replay current level for better score
void VictoryState::onReplay() {
    if (m_game && m_machine)
        m_machine->changeState(std::make_unique<PlayState>(m_game, m_machine, m_currentLevel));
}