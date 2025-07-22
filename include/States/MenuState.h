// ================================
// Menu State - State Pattern with Animated Main Menu Interface
// ================================
#pragma once
#include "States/IState.h"
#include "Core/Constants.h"
#include <SFML/Graphics.hpp>
#include "States/StateMachine.h"
#include "Core/Game.h"
#include "UI/Button.h"
#include <vector>
#include <memory>

class AudioManager;
class StateMachine;
class Game;

// Main menu state with animated elements and audio integration
class MenuState : public IState {
public:
    MenuState(Game* game, StateMachine* machine);
    virtual ~MenuState() = default;

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

    // UI Elements
    std::vector<std::unique_ptr<Button>> m_buttons;          // Main menu buttons
    sf::Text m_titleText;                                   // Game title text
    sf::Text m_versionText;                                 // Version information
    sf::Text m_creditsText;                                 // Credits information

    // Background System
    sf::Sprite m_backgroundSprite;                          // Background image sprite
    sf::Texture m_backgroundTexture;                        // Background texture
    sf::RectangleShape m_backgroundOverlay;                 // Semi-transparent overlay for text readability

    // Menu Navigation State
    int m_selectedButtonIndex = 0;                          // Currently selected button (keyboard navigation)
    bool m_showCredits = false;                             // Credits display toggle

    // Animation System
    float m_titleAnimationTime = 0.0f;                      // Title animation timer
    float m_buttonHoverScale = 1.0f;                        // Button hover scale effect

    // Audio Integration
    AudioManager* m_audioManager = nullptr;                 // Reference to audio system

    // Setup and Initialization
    void setupUI();                                         // Initialize all UI elements
    void setupButtons();                                    // Create and configure menu buttons
    void setupBackground();                                 // Load and configure background

    // Input and Navigation
    void updateButtonSelection(int direction);              // Handle keyboard navigation
    void updateAnimations(float dt);                        // Update visual animations
    void updateButtonHover(const sf::Vector2f& mousePos);   // Handle mouse hover effects

    // Rendering Methods
    void renderBackground(sf::RenderWindow& window);        // Render background elements
    void renderUI(sf::RenderWindow& window);                // Render UI elements
};