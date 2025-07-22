#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "States/StateMachine.h"
#include "States/MenuState.h"
#include "Core/Constants.h"
#include "Utils/ConfigLoader.h"
#include "Managers/AudioManager.h"

class Game {
public:
    Game();                                  // Constructor
    ~Game();                                 // Destructor

    void run();                              // Start the game loop

    void changeState(std::unique_ptr<class IState> newState);     // Change current game state

    sf::RenderWindow& getWindow();           // Get window (modifiable)
    const sf::RenderWindow& getWindow() const;  // Get window (const)

    void quit();                             // Quit the game
    bool isRunning() const;                  // Check if game is running

    void applySettingsChanges();             // Apply updated settings
    void loadInitialSettings();              // Load initial game settings

private:
    void processEvents();                    // Handle window events
    void update(float dt);                   // Update game logic
    void render();                           // Render the current state
    void calculateDeltaTime();               // Calculate frame delta time

    void recreateWindow();                   // Recreate window with new settings
    void updateWindowSettings();             // Update window-related configurations

    sf::RenderWindow m_window;               // Game window
    std::unique_ptr<class StateMachine> m_stateMachine;  // State machine for game states

    sf::Clock m_clock;                       // Clock for timing
    float m_deltaTime = 0.0f;                 // Time between frames
    float m_accumulator = 0.0f;               // Time accumulator for fixed updates

    bool m_isRunning = true;                 // Running state flag

    static constexpr float MAX_DELTA_TIME = 0.05f;    // Max delta to prevent large frame jumps
};
