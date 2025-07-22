// ================================
// State Machine - State Pattern Implementation with Stack Management and Deferred Operations
// ================================
#include "States/StateMachine.h"
#include "States/IState.h"
#include "Core/Game.h"

// Constructor - Initialize state machine with game reference
StateMachine::StateMachine(Game* game)
    : m_game(game) {
}

// Destructor - Ensure all states are properly cleaned up
StateMachine::~StateMachine() {
    clearStates();
}

// Event handling - route events to current state and process deferred operations
void StateMachine::handleEvent(const sf::Event& event) {
    if (!m_states.empty()) {
        m_states.top()->handleEvent(event);
    }
    processPendingOperations();  // Handle state changes after event processing
}

// Update current state and process deferred operations
void StateMachine::update(float dt) {
    if (!m_states.empty() && !m_states.top()->isPaused()) {
        m_states.top()->update(dt);
    }
    processPendingOperations();  // Handle state changes after update
}

// Render current state (supports transparent state layering)
void StateMachine::render(sf::RenderWindow& window) {
    if (!m_states.empty()) {
        // For future enhancement: could render multiple states if transparent
        m_states.top()->render(window);
    }
}

// Deferred State Management Operations (prevents stack modification during iteration)

// Queue push operation for deferred execution
void StateMachine::pushState(std::unique_ptr<IState> state) {
    m_pendingOperations.emplace_back(PendingOperation::Push, std::move(state));
}

// Queue pop operation for deferred execution
void StateMachine::popState() {
    m_pendingOperations.emplace_back(PendingOperation::Pop);
}

// Queue state change operation for deferred execution
void StateMachine::changeState(std::unique_ptr<IState> state) {
    m_pendingOperations.emplace_back(PendingOperation::Change, std::move(state));
}

// Immediately clear all states (called during shutdown)
void StateMachine::clearStates() {
    while (!m_states.empty()) {
        m_states.top()->onExit();
        m_states.pop();
    }
}

// Execute all queued state operations safely
void StateMachine::processPendingOperations() {
    for (auto& operation : m_pendingOperations) {
        switch (operation.type) {
        case PendingOperation::Push:
            // Pause current state before pushing new one
            if (!m_states.empty()) {
                m_states.top()->onPause();
            }
            // Setup and activate new state
            setupState(operation.state.get());
            m_states.push(std::move(operation.state));
            m_states.top()->onEnter();
            break;

        case PendingOperation::Pop:
            // Exit current state
            if (!m_states.empty()) {
                m_states.top()->onExit();
                m_states.pop();
            }
            // Resume previous state if it exists
            if (!m_states.empty()) {
                m_states.top()->onResume();
            }
            break;

        case PendingOperation::Change:
            // Exit current state completely
            if (!m_states.empty()) {
                m_states.top()->onExit();
                m_states.pop();
            }
            // Setup and activate replacement state
            setupState(operation.state.get());
            m_states.push(std::move(operation.state));
            m_states.top()->onEnter();
            break;

        case PendingOperation::Clear:
            clearStates();
            break;
        }
    }
    m_pendingOperations.clear();  // Clear processed operations
}

// State Query Methods

// Get currently active state (top of stack)
IState* StateMachine::getCurrentState() const {
    return m_states.empty() ? nullptr : m_states.top().get();
}

// Get previous state (not directly available in stack implementation)
IState* StateMachine::getPreviousState() const {
    return nullptr;  // Stack doesn't expose previous states by default
}

// Check if any states exist in the stack
bool StateMachine::hasStates() const noexcept {
    return !m_states.empty();
}

// Get total number of states in stack
int StateMachine::getStateCount() const noexcept {
    return static_cast<int>(m_states.size());
}

// State Control Operations

// Pause current state (for overlay states)
void StateMachine::pauseCurrentState() {
    if (!m_states.empty()) {
        m_states.top()->onPause();
    }
}

// Resume current state (after overlay is removed)
void StateMachine::resumeCurrentState() {
    if (!m_states.empty()) {
        m_states.top()->onResume();
    }
}

// Check if current state is paused
bool StateMachine::isCurrentStatePaused() const noexcept {
    return !m_states.empty() && m_states.top()->isPaused();
}

// Private Helper Methods

// Setup state with game reference for system access
void StateMachine::setupState(IState* state) {
    if (state && m_game) {
        state->setGame(m_game);
    }
}