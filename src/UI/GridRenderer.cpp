// ================================
// src/UI/GridRenderer.cpp 
// ================================
#include "UI/GridRenderer.h"
#include "Managers/ResourceManager.h"
#include "Core/StaticObject.h"

GridRenderer::GridRenderer() {
    // Constructor is now clean
}

void GridRenderer::update(float dt) {
    // No-op - grid is static
}

void GridRenderer::draw(sf::RenderWindow& window) const {
    // Always draw placement slots using texture
    auto& rm = ResourceManager::getInstance();
    sf::Texture& placementTexture = rm.getTexture("placing_slot.png");

    for (int lane = 0; lane < m_gridHeight; ++lane) {
        for (int gridX = 0; gridX < m_gridWidth; ++gridX) {
            if (!isValidGridPosition(gridX, lane)) continue;

            sf::Vector2f center = gridToWorldPosition(gridX, lane);
            center.x += m_cellSize.x / 2.f;
            center.y += m_cellSize.y / 2.f + 7.0f; // Slight downward offset

            sf::Sprite slot;
            slot.setTexture(placementTexture);

            sf::Vector2u textureSize = placementTexture.getSize();
            slot.setOrigin(textureSize.x / 2.f, textureSize.y / 2.f);
            slot.setPosition(center);
            slot.setScale(0.12f, 0.08f);
            slot.setColor(sf::Color(255, 255, 255, 200)); // Always visible

            window.draw(slot);
        }
    }
}

// Grid utilities
sf::Vector2i GridRenderer::getGridPosition(const sf::Vector2f& mousePos) const {
    int gridX = static_cast<int>((mousePos.x - m_gridOffset.x) / m_cellSize.x);
    int lane = static_cast<int>((mousePos.y - m_gridOffset.y) / m_cellSize.y);
    return sf::Vector2i(gridX, lane);
}

sf::Vector2f GridRenderer::gridToWorldPosition(int gridX, int lane) const {
    return StaticObject::gridToWorldPosition(gridX, lane);
}

sf::Vector2i GridRenderer::worldToGridPosition(const sf::Vector2f& worldPos) const {
    return StaticObject::worldToGridPosition(worldPos);
}

bool GridRenderer::isValidPlacement(const sf::Vector2i& gridPos) const {
    return StaticObject::isValidGridPosition(gridPos.x, gridPos.y) && !isCellBlocked(gridPos);
}

bool GridRenderer::isValidGridPosition(int gridX, int lane) const {
    return StaticObject::isValidGridPosition(gridX, lane);
}

// Grid configuration
void GridRenderer::setGridSize(int width, int height) {
    m_gridWidth = width;
    m_gridHeight = height;
}

sf::Vector2i GridRenderer::getGridSize() const {
    return sf::Vector2i(m_gridWidth, m_gridHeight);
}

void GridRenderer::setGridOffset(const sf::Vector2f& offset) {
    m_gridOffset = offset;
}

void GridRenderer::setCellSize(const sf::Vector2f& cellSize) {
    m_cellSize = cellSize;
}

// Block/unblock cells
void GridRenderer::blockCell(const sf::Vector2i& gridPos) {
    m_blockedCells.insert(gridPos);
}

void GridRenderer::unblockCell(const sf::Vector2i& gridPos) {
    m_blockedCells.erase(gridPos);
}

bool GridRenderer::isCellBlocked(const sf::Vector2i& gridPos) const {
    return m_blockedCells.count(gridPos) > 0;
}