// ================================
// State Machine - State Pattern with Stack-Based State Management
// ================================
#pragma once
#include <memory>
#include <stack>
#include <vector>
#include <SFML/Graphics.hpp>
#include "States/IState.h"

// Forward declarations
class Game;

// Central state management system using stack-based state transitions
class StateMachine {
public:
    StateMachine(Game* game);
    ~StateMachine();

    // Core Update Loop
    void handleEvent(const sf::Event& event);                // Route events to current state
    void update(float dt);                                   // Update current state
    void render(sf::RenderWindow& window);                   // Render current state(s)

    // State Stack Management
    void pushState(std::unique_ptr<IState> state);           // Add state to top of stack
    void popState();                                         // Remove top state from stack
    void changeState(std::unique_ptr<IState> state);         // Replace current state
    void clearStates();                                      // Remove all states

    // State Query Methods
    IState* getCurrentState() const;                         // Get currently active state
    IState* getPreviousState() const;                        // Get previously active state
    bool hasStates() const noexcept;                         // Check if any states exist
    int getStateCount() const noexcept;                      // Get number of states in stack

    // State Control Operations
    void pauseCurrentState();                                // Pause current state
    void resumeCurrentState();                               // Resume current state
    bool isCurrentStatePaused() const noexcept;              // Check if current state is paused

private:
    // Core References
    Game* m_game;                                            // Reference to main game instance
    std::stack<std::unique_ptr<IState>> m_states;           // Stack of active states
    IState* m_lastPoppedState = nullptr;                     // Cache of last removed state

    // Deferred Operation System (prevents stack modification during iteration)
    struct PendingOperation {
        enum Type { Push, Pop, Change, Clear };              // Operation types
        Type type;                                           // Operation to perform
        std::unique_ptr<IState> state;                       // State for push/change operations

        // Convenience constructors
        PendingOperation(Type t) : type(t) {}
        PendingOperation(Type t, std::unique_ptr<IState> s) : type(t), state(std::move(s)) {}
    };
    std::vector<PendingOperation> m_pendingOperations;       // Queue of deferred operations

    // Internal Management
    void processPendingOperations();                         // Execute queued state operations
    void setupState(IState* state);                          // Initialize new state with game reference
};