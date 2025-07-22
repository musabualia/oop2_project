// ================================
// Splash State - State Pattern with Multithreading for Asset Preloading
// ================================
#pragma once
#include "States/IState.h"
#include <SFML/Graphics.hpp>
#include <thread>
#include <atomic>

class StateMachine;
class Game;

// Initial loading screen with animated transitions and background asset loading
class SplashState : public IState {
public:
    SplashState(Game* game, StateMachine* machine);
    virtual ~SplashState();

    // Core state interface implementation
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

    // State lifecycle management
    void onEnter() override;
    void onExit() override;

private:
    // Core system references
    Game* m_game;
    StateMachine* m_machine;

    // Visual Elements
    sf::Sprite m_backgroundSprite;                            // Background image
    sf::Texture m_backgroundTexture;                          // Background texture

    // Text Elements
    sf::Font m_orbitronFont;                                  // Custom font for splash screen
    sf::Text m_createdByText;                                 // "Created by" text
    sf::Text m_nameText1;                                     // Developer name 1
    sf::Text m_nameText2;                                     // Developer name 2
    sf::Text m_loadingText;                                   // "Loading..." text

    // Animation Timing System
    float m_totalTime = 0.0f;                                 // Total elapsed time
    float m_creditsPhaseTime = 2.0f;                          // Duration for credits display
    float m_loadingPhaseTime = 3.0f;                          // Duration for loading display
    float m_fadeTime = 0.5f;                                  // Fade transition duration

    // Animation State Machine
    enum class Phase {
        CreditsIn,     // Credits text fading in
        CreditsStay,   // Credits text visible and stable
        CreditsOut,    // Credits text fading out
        LoadingIn,     // Loading text fading in
        LoadingStay,   // Loading text visible with loading progress
        LoadingOut,    // Loading text fading out
        Complete       // Ready to transition to main menu
    };
    Phase m_currentPhase = Phase::CreditsIn;

    // Multithreading for Asset Loading
    std::thread m_loadingThread;                              // Background loading thread
    std::atomic<bool> m_assetsLoaded{ false };                // Thread-safe loading completion flag
    std::atomic<bool> m_shouldStopLoading{ false };           // Thread-safe shutdown flag

    // Animation and Setup Methods
    void setupBackground();                                   // Initialize background graphics
    void setupText();                                         // Initialize all text elements
    void updateAnimation(float dt);                           // Update animation state and timers
    void updatePhase();                                       // Transition between animation phases

    // Background Loading System
    void startBackgroundLoading();                           // Start asset loading thread
    void loadAssetsInBackground();                            // Thread function for asset loading
    void transitionToMenu();                                  // Transition to main menu when complete

    // Rendering Helpers
    void renderTextWithAlpha(sf::RenderWindow& window, sf::Text& text, float alpha);  // Render text with fade
    float calculateAlpha();                                   // Calculate current fade alpha value

    // Text Positioning
    void positionCreditsText();                               // Center credits text on screen
    void positionLoadingText();                               // Center loading text on screen
};