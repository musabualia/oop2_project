// ================================
// IState - Abstract State Interface for State Pattern Implementation
// ================================
#pragma once
#include <SFML/Graphics.hpp>

class Game;

// Abstract base class defining the interface for all game states
// Implements the State Pattern for game flow management
class IState {
public:
    virtual ~IState() = default;

    // Core State Interface (must be implemented by all states)
    virtual void handleEvent(const sf::Event& event) = 0;   // Process input events
    virtual void update(float dt) = 0;                      // Update state logic
    virtual void render(sf::RenderWindow& window) = 0;      // Render state visuals

    // State Lifecycle Hooks (optional overrides)
    virtual void onEnter() {}                               // Called when state becomes active
    virtual void onExit() {}                                // Called when state is removed
    virtual void onPause() {}                               // Called when state is paused (overlay states)
    virtual void onResume() {}                              // Called when state resumes from pause

    // Game Reference Management
    void setGame(Game* game);                               // Set reference to main game instance
    Game* getGame() const;                                  // Get reference to main game instance

    // State Behavior Queries
    virtual bool isPaused() const { return false; }         // Check if state is currently paused
    virtual bool isTransparent() const { return false; }    // Check if underlying states should render

protected:
    Game* m_game = nullptr;                                 // Reference to main game instance for system access
};