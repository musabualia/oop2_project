// ================================
// Splash State - Multithreaded Asset Loading with Animated Fade Transitions
// ================================
#include "States/SplashState.h"
#include "States/MenuState.h"
#include "States/StateMachine.h"
#include "Managers/ResourceManager.h"
#include "Managers/AudioManager.h"
#include "Core/Game.h"
#include <iostream>

// Constructor - Initialize splash screen with background and text elements
SplashState::SplashState(Game* game, StateMachine* machine)
    : m_game(game), m_machine(machine) {
    setupBackground();
    setupText();
    positionCreditsText();
    positionLoadingText();
}

// Destructor - Ensure background thread is properly terminated
SplashState::~SplashState() {
    m_shouldStopLoading = true;
    if (m_loadingThread.joinable()) {
        m_loadingThread.join();
    }
}

// Initialize background image with fallback handling
void SplashState::setupBackground() {
    if (!m_backgroundTexture.loadFromFile("welcome_bg.png")) {
        std::cerr << "Error: Could not load welcome_bg.png" << std::endl;
        // Create fallback orange background
        sf::Image fallbackImage;
        fallbackImage.create(800, 600, sf::Color(255, 165, 0));
        m_backgroundTexture.loadFromImage(fallbackImage);
    }
    m_backgroundSprite.setTexture(m_backgroundTexture);
}

// Initialize text elements with font fallback system
void SplashState::setupText() {
    // Try to load custom font, fallback to system font
    if (!m_orbitronFont.loadFromFile("Orbitron-VariableFont_wght.ttf")) {
        std::cerr << "Error: Could not load Orbitron font" << std::endl;
        if (!m_orbitronFont.loadFromFile("DejaVuSans.ttf")) {
            std::cerr << "Error: Could not load any font" << std::endl;
        }
    }

    // Setup credits text elements
    m_createdByText.setFont(m_orbitronFont);
    m_createdByText.setString("Created by:");
    m_createdByText.setCharacterSize(28);
    m_createdByText.setFillColor(sf::Color::Black);
    m_createdByText.setStyle(sf::Text::Bold);

    m_nameText1.setFont(m_orbitronFont);
    m_nameText1.setString("Musa Abu Alia");
    m_nameText1.setCharacterSize(32);
    m_nameText1.setFillColor(sf::Color::Black);
    m_nameText1.setStyle(sf::Text::Bold);

    m_nameText2.setFont(m_orbitronFont);
    m_nameText2.setString("& Ellen Habash");
    m_nameText2.setCharacterSize(32);
    m_nameText2.setFillColor(sf::Color::Black);
    m_nameText2.setStyle(sf::Text::Bold);

    // Setup loading text
    m_loadingText.setFont(m_orbitronFont);
    m_loadingText.setString("PREPARING BATTLEFIELD...");
    m_loadingText.setCharacterSize(36);
    m_loadingText.setFillColor(sf::Color::Black);
}

// Position credits text elements in attractive layout
void SplashState::positionCreditsText() {
    sf::Vector2u windowSize = m_game->getWindow().getSize();
    sf::Vector2f winSizeF(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y));

    float centerY = winSizeF.y * 0.15f;
    float lineSpacing = 45.0f;

    // Position with slight offset for visual appeal
    sf::FloatRect createdByBounds = m_createdByText.getLocalBounds();
    m_createdByText.setPosition(
        winSizeF.x / 2.0f - createdByBounds.width / 2.0f + 100.0f,
        centerY - lineSpacing
    );

    sf::FloatRect name1Bounds = m_nameText1.getLocalBounds();
    m_nameText1.setPosition(
        winSizeF.x / 2.0f - name1Bounds.width / 2.0f + 50.0f,
        centerY
    );

    sf::FloatRect name2Bounds = m_nameText2.getLocalBounds();
    m_nameText2.setPosition(
        winSizeF.x / 2.0f - name2Bounds.width / 2.0f + 150.0f,
        centerY + lineSpacing
    );
}

// Position loading text in upper area of screen
void SplashState::positionLoadingText() {
    sf::Vector2u windowSize = m_game->getWindow().getSize();
    sf::Vector2f winSizeF(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y));

    sf::FloatRect loadingBounds = m_loadingText.getLocalBounds();
    m_loadingText.setPosition(
        winSizeF.x / 2.0f - loadingBounds.width / 2.0f,
        winSizeF.y * 0.10f - loadingBounds.height / 2.0f  // Upper middle area
    );
}

// Handle input events (currently just allow skipping)
void SplashState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed) {
        // Could add skip functionality here if desired
    }
}

// Main update loop - manages animation phases and completion detection
void SplashState::update(float dt) {
    m_totalTime += dt;
    updateAnimation(dt);
    updatePhase();

    // Transition to menu when both animation and loading are complete
    if (m_currentPhase == Phase::Complete && m_assetsLoaded) {
        transitionToMenu();
    }
}

// Update animation state (placeholder for additional effects)
void SplashState::updateAnimation(float) {}

// State machine for animation phase transitions
void SplashState::updatePhase() {
    float phaseTime = 0.0f;

    switch (m_currentPhase) {
    case Phase::CreditsIn:
        phaseTime = m_fadeTime;
        if (m_totalTime >= phaseTime) {
            m_currentPhase = Phase::CreditsStay;
            m_totalTime = 0.0f;
        }
        break;

    case Phase::CreditsStay:
        phaseTime = m_creditsPhaseTime - m_fadeTime;
        if (m_totalTime >= phaseTime) {
            m_currentPhase = Phase::CreditsOut;
            m_totalTime = 0.0f;
        }
        break;

    case Phase::CreditsOut:
        phaseTime = m_fadeTime;
        if (m_totalTime >= phaseTime) {
            m_currentPhase = Phase::LoadingIn;
            m_totalTime = 0.0f;
        }
        break;

    case Phase::LoadingIn:
        phaseTime = m_fadeTime;
        if (m_totalTime >= phaseTime) {
            m_currentPhase = Phase::LoadingStay;
            m_totalTime = 0.0f;
        }
        break;

    case Phase::LoadingStay:
        phaseTime = m_loadingPhaseTime - m_fadeTime;
        if (m_totalTime >= phaseTime) {
            m_currentPhase = Phase::LoadingOut;
            m_totalTime = 0.0f;
        }
        break;

    case Phase::LoadingOut:
        phaseTime = m_fadeTime;
        if (m_totalTime >= phaseTime) {
            m_currentPhase = Phase::Complete;
            m_totalTime = 0.0f;
        }
        break;

    case Phase::Complete:
        // Wait for asset loading completion
        break;
    }
}

// Calculate fade alpha value based on current phase and timing
float SplashState::calculateAlpha() {
    switch (m_currentPhase) {
    case Phase::CreditsIn:
        return (m_totalTime / m_fadeTime) * 255.0f;
    case Phase::CreditsStay:
        return 255.0f;
    case Phase::CreditsOut:
        return (1.0f - m_totalTime / m_fadeTime) * 255.0f;
    case Phase::LoadingIn:
        return (m_totalTime / m_fadeTime) * 255.0f;
    case Phase::LoadingStay:
        return 255.0f;
    case Phase::LoadingOut:
        return (1.0f - m_totalTime / m_fadeTime) * 255.0f;
    default:
        return 0.0f;
    }
}

// Render splash screen with appropriate content based on current phase
void SplashState::render(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u textureSize = m_backgroundSprite.getTexture()->getSize();

    // Scale background to fit window
    m_backgroundSprite.setScale(
        static_cast<float>(windowSize.x) / textureSize.x,
        static_cast<float>(windowSize.y) / textureSize.y
    );

    // Update text positions for current window size
    positionCreditsText();
    positionLoadingText();

    window.draw(m_backgroundSprite);

    float alpha = calculateAlpha();

    // Render credits during credits phases
    if (m_currentPhase == Phase::CreditsIn ||
        m_currentPhase == Phase::CreditsStay ||
        m_currentPhase == Phase::CreditsOut) {
        renderTextWithAlpha(window, m_createdByText, alpha);
        renderTextWithAlpha(window, m_nameText1, alpha);
        renderTextWithAlpha(window, m_nameText2, alpha);
    }

    // Render loading text during loading phases
    if (m_currentPhase == Phase::LoadingIn ||
        m_currentPhase == Phase::LoadingStay ||
        m_currentPhase == Phase::LoadingOut) {
        renderTextWithAlpha(window, m_loadingText, alpha);
    }
}

// Render text with specified alpha transparency
void SplashState::renderTextWithAlpha(sf::RenderWindow& window, sf::Text& text, float alpha) {
    sf::Color color = text.getFillColor();
    color.a = static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, alpha)));
    text.setFillColor(color);
    window.draw(text);
    // Restore original alpha
    color.a = 255;
    text.setFillColor(color);
}

// State lifecycle - start background music and asset loading
void SplashState::onEnter() {
    AudioManager::getInstance().playMusic("Menu11", true);
    startBackgroundLoading();
}

// State lifecycle - cleanup audio and ensure thread termination
void SplashState::onExit() {
    AudioManager::getInstance().stopMusic();
    m_shouldStopLoading = true;
    if (m_loadingThread.joinable()) {
        m_loadingThread.join();
    }
}

// Multithreading System

// Start background asset loading thread
void SplashState::startBackgroundLoading() {
    m_loadingThread = std::thread(&SplashState::loadAssetsInBackground, this);
}

// Background thread function for asset preloading
void SplashState::loadAssetsInBackground() {
    try {
        ResourceManager& rm = ResourceManager::getInstance();

        // Preload all game assets in background
        rm.preloadMenuAssets();     // Menu textures and UI elements
        rm.preloadGameAssets();     // Gameplay textures and sounds

        m_assetsLoaded = true;      // Signal completion
    }
    catch (const std::exception& e) {
        std::cerr << "Background loading error: " << e.what() << std::endl;
        m_assetsLoaded = true;      // Continue even if some assets failed
    }
}

// Transition to main menu when loading is complete
void SplashState::transitionToMenu() {
    m_machine->changeState(std::make_unique<MenuState>(m_game, m_machine));
}