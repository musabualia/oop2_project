// ================================
// Settings State - State Pattern for Game Configuration Management
// ================================
#pragma once

#include "States/IState.h"
#include "UI/Button.h"
#include "UI/Slider.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Game;
class StateMachine;

// Settings configuration state with audio/video/control options
class SettingsState : public IState {
public:
    SettingsState(Game* game, StateMachine* machine);
    ~SettingsState() override = default;

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
    sf::Text m_titleText;                                     // "SETTINGS" title
    sf::Sprite m_backgroundSprite;                            // Background image
    std::vector<std::unique_ptr<Button>> m_actionButtons;     // Back/Apply buttons
    std::vector<std::unique_ptr<Slider>> m_volumeSliders;     // Volume control sliders

    // UI Management
    void setupUI();                         // Initialize all UI elements and layout

    // Button action handlers
    void onBack();                          // Return to previous state without saving
    void onApply();                         // Apply settings and return to previous state
};