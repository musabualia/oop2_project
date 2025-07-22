// ================================
// IState.cpp - Abstract State Interface Implementation
// ================================
#include "States/IState.h"
#include "Core/Game.h"

// Set reference to main game instance for system access
void IState::setGame(Game* game) {
    m_game = game;
}

// Get reference to main game instance
Game* IState::getGame() const {
    return m_game;
}