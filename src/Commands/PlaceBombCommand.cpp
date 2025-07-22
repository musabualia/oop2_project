#include "Commands/PlaceBombCommand.h"
#include "Entities/Projectiles/Bomb.h"
#include <sstream>
#include <iostream>

PlaceBombCommand::PlaceBombCommand(std::vector<Bomb>* bombList, const sf::Vector2f& position)
    : m_bombList(bombList)
    , m_position(position)
    , m_bombIndex(SIZE_MAX)  // Initialize to invalid index
    , m_bombPlaced(false) {
}

void PlaceBombCommand::execute() {
    if (m_executed || !m_bombList) {
        return;
    }

    // Create bomb and store its index
    m_bombList->emplace_back(m_position);
    m_bombIndex = m_bombList->size() - 1;  // Store index instead of iterator

    m_bombPlaced = true;
    m_executed = true;
}

void PlaceBombCommand::undo() {
    if (!m_executed || !m_bombPlaced || !m_bombList) {
        return;
    }

    if (!isBombStillValid()) {
        m_bombPlaced = false;
        return;
    }

    // Check if bomb has exploded
    Bomb* bomb = getBombAtIndex();
    if (bomb && bomb->hasExploded()) {
        m_bombPlaced = false;
    }

    if (m_bombIndex < m_bombList->size()) {
        m_bombList->erase(m_bombList->begin() + m_bombIndex);
    }

    m_bombPlaced = false;
    m_executed = false;
    m_bombIndex = SIZE_MAX;  // Invalidate index
}

bool PlaceBombCommand::canUndo() const {
    return m_executed &&
        m_bombPlaced &&
        m_bombList &&
        isBombStillValid() &&
        !hasExploded();
}

std::string PlaceBombCommand::getDescription() const {
    std::ostringstream desc;
    return desc.str();
}

bool PlaceBombCommand::hasExploded() const {
    if (!m_bombPlaced || !isBombStillValid()) {
        return true;  // Consider invalid bombs as "exploded"
    }

    Bomb* bomb = getBombAtIndex();
    return bomb ? bomb->hasExploded() : true;
}

bool PlaceBombCommand::isBombStillValid() const {
    return m_bombList &&
        m_bombIndex != SIZE_MAX &&
        m_bombIndex < m_bombList->size();
}

Bomb* PlaceBombCommand::getBombAtIndex() const {
    if (isBombStillValid()) {
        return &(*m_bombList)[m_bombIndex];
    }
    return nullptr;
}