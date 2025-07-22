// ================================
// Pause State - State Pattern for Game Pause and Resume Management
// ================================
#pragma once

#include "States/IState.h"
#include "Core/Constants.h"
#include "UI/Button.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Game;
class StateMachine;

// Transparent overlay state that pauses gameplay while showing menu options
class PauseState : public IState {
public:
    PauseState(Game* game, StateMachine* machine);
    virtual ~PauseState() = default;

    // Core state interface implementation
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

    // State lifecycle management
    void onEnter() override;
    void onExit() override;

    // Transparent state - renders over underlying game state
    bool isTransparent() const override { return true; }

private:
    // Core system references
    Game* m_game;
    StateMachine* m_machine;

    // UI Elements
    std::vector<std::unique_ptr<Button>> m_buttons;    // Pause menu buttons
    sf::Text m_pauseTitle;                             // "GAME PAUSED" title
    sf::Sprite m_backgroundSprite;                     // Semi-transparent overlay

    // Navigation system
    int m_selectedButtonIndex = 0;                     // Current selected button for keyboard navigation

    // Button action handlers
    void onResumeButton();      // Resume gameplay
    void onSettingsButton();    // Open settings menu
    void onMainMenuButton();    // Return to main menu (with confirmation)
    void onQuitButton();        // Quit to desktop (with confirmation)

    // UI Management
    void setupUI();                                           // Initialize all UI elements
    void setupButtons();                                      // Create and position buttons
    void updateButtonSelection(int direction);                // Handle keyboard navigation
    void updateButtonHover(const sf::Vector2f& mousePos);     // Handle mouse hover
    void renderOverlay(sf::RenderWindow& window);             // Render semi-transparent background
    void renderMenu(sf::RenderWindow& window);                // Render pause menu content
};