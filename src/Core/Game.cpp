// ================================
// Game Class - Main Application Controller and Game Loop
// ================================
#include "Core/Game.h"
#include "Factories/SquadMemberFactory.h" 
#include "Entities/Base/Projectile.h"
#include "Entities/Projectiles/Bullet.h"
#include "Entities/Projectiles/SniperBullet.h"
#include "States/MenuState.h"
#include "Managers/ResourceManager.h"
#include "Systems/AnimationSystem.h"
#include "Systems/EventSystem.h"
#include "Managers/SettingsManager.h"
#include "States/SplashState.h"
#include "Utils/ConfigLoader.h" 
#include <iostream>

// ================================
// Constructor - Initialize window and core systems
// ================================
Game::Game()
    : m_window(sf::VideoMode(1200, 800), "Special Forces vs Robots")
    , m_stateMachine(std::make_unique<StateMachine>(this))
{
    auto& config = ConfigLoader::getInstance();

    // Load settings and configure window
    loadInitialSettings();
    updateWindowSettings();

    // Initialize core game systems
    AudioManager::getInstance().initialize();
    ResourceManager::getInstance().preloadGameAssets();
    ResourceManager::getInstance().preloadMenuAssets();
    AnimationSystem::getInstance().initialize();
    SquadMemberFactory::getInstance().initialize();

    // Start with splash screen using State pattern
    m_stateMachine->pushState(std::make_unique<SplashState>(this, m_stateMachine.get()));
}

// ================================
// Destructor - Clean shutdown of systems
// ================================
Game::~Game() {
    AnimationSystem::getInstance().shutdown();
    AudioManager::getInstance().shutdown();
}

// ================================
// Main Game Loop - Core application loop
// ================================
void Game::run() {
    while (m_isRunning && m_window.isOpen()) {
        applySettingsChanges();    // Handle any settings changes
        calculateDeltaTime();      // Calculate frame timing
        processEvents();           // Handle input events
        update(m_deltaTime);       // Update game logic
        render();                  // Draw everything to screen
    }
}

// ================================
// Event Processing - Handle SFML window events
// ================================
void Game::processEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        // Handle window close button
        if (event.type == sf::Event::Closed) {
            quit();
        }

        // Pass events to current game state
        if (m_stateMachine) {
            m_stateMachine->handleEvent(event);
        }
    }
}

// ================================
// Game Logic Update - Update all game systems
// ================================
void Game::update(float dt) {
    // Update current game state (menu, gameplay, pause, etc.)
    if (m_stateMachine) {
        m_stateMachine->update(dt);
    }

    // Update global systems that run in all states
    AnimationSystem::getInstance().update(dt);
    EventSystem::getInstance().processEvents();
}

// ================================
// Rendering - Draw current game state
// ================================
void Game::render() {
    m_window.clear();

    // Render current state (delegates to MenuState, PlayState, etc.)
    if (m_stateMachine) {
        m_stateMachine->render(m_window);
    }

    m_window.display();
}

// ================================
// Frame Timing - Calculate delta time with safety cap
// ================================
void Game::calculateDeltaTime() {
    m_deltaTime = m_clock.restart().asSeconds();

    // Prevent huge frame spikes that could break physics
    if (m_deltaTime > MAX_DELTA_TIME) {
        m_deltaTime = MAX_DELTA_TIME;
    }
}

// ================================
// State Management - Change game state
// ================================
void Game::changeState(std::unique_ptr<IState> newState) {
    if (m_stateMachine) {
        m_stateMachine->changeState(std::move(newState));
    }
}

// ================================
// Window Access - Get window reference for states
// ================================
sf::RenderWindow& Game::getWindow() {
    return m_window;
}

const sf::RenderWindow& Game::getWindow() const {
    return m_window;
}

// ================================
// Application Control
// ================================
void Game::quit() {
    m_isRunning = false;
    m_window.close();
}

bool Game::isRunning() const {
    return m_isRunning && m_window.isOpen();
}

// ================================
// Settings Management - Load and apply game settings
// ================================
void Game::loadInitialSettings() {
    auto& settingsManager = SettingsManager::getInstance();
    SettingsManager::AllSettings settings;

    // Load settings from file and apply them
    settingsManager.loadAllSettings(settings);
    settingsManager.applyAllSettings(settings);
}

void Game::applySettingsChanges() {
    auto& settingsManager = SettingsManager::getInstance();

    // Check if settings changed and need to be applied
    if (settingsManager.hasSettingsChanged()) {
        updateWindowSettings();
        settingsManager.markSettingsApplied();
    }
}

// ================================
// Window Management - Recreate window with new settings
// ================================
void Game::recreateWindow() {
    auto& settingsManager = SettingsManager::getInstance();

    // Get current settings
    sf::Vector2u resolution = settingsManager.getResolution();
    bool fullscreen = settingsManager.isFullscreen();
    bool vsync = settingsManager.isVSync();

    // Create new window with updated settings
    sf::VideoMode mode(resolution.x, resolution.y);
    sf::Uint32 style = fullscreen ? sf::Style::Fullscreen : sf::Style::Default;

    m_window.create(mode, "Special Forces vs Robots", style);
    m_window.setFramerateLimit(60);
    m_window.setVerticalSyncEnabled(vsync);
}

void Game::updateWindowSettings() {
    auto& settingsManager = SettingsManager::getInstance();

    // Check if window needs to be recreated (resolution change)
    sf::Vector2u currentSize = m_window.getSize();
    sf::Vector2u targetResolution = settingsManager.getResolution();
    bool needsRecreation = (currentSize != targetResolution);

    if (needsRecreation) {
        recreateWindow();
    }
    else {
        // Just update VSync if resolution didn't change
        m_window.setVerticalSyncEnabled(settingsManager.isVSync());
    }
}