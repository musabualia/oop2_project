// ================================
// include/UI/GridRenderer.h
// ================================
#pragma once
#include "Core/Constants.h"
#include <SFML/Graphics.hpp>
#include <unordered_set>

struct Vector2iHash {
    std::size_t operator()(const sf::Vector2i& v) const {
        return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
    }
};

class GridRenderer {
public:
    GridRenderer();
    ~GridRenderer() = default;

    // Core rendering
    void update(float dt);
    void draw(sf::RenderWindow& window) const;

    // Grid utilities
    sf::Vector2i getGridPosition(const sf::Vector2f& mousePos) const;
    sf::Vector2f gridToWorldPosition(int gridX, int lane) const;
    sf::Vector2i worldToGridPosition(const sf::Vector2f& worldPos) const;
    bool isValidPlacement(const sf::Vector2i& gridPos) const;
    bool isValidGridPosition(int gridX, int lane) const;

    // Grid configuration
    void setGridSize(int width, int height);
    sf::Vector2i getGridSize() const;
    void setGridOffset(const sf::Vector2f& offset);
    void setCellSize(const sf::Vector2f& cellSize);

    // Block/unblock cells
    void blockCell(const sf::Vector2i& gridPos);
    void unblockCell(const sf::Vector2i& gridPos);
    bool isCellBlocked(const sf::Vector2i& gridPos) const;

private:
    int m_gridWidth = GameConstants::GRID_COLUMNS;
    int m_gridHeight = GameConstants::GRID_ROWS;
    sf::Vector2f m_gridOffset = sf::Vector2f(GameConstants::GRID_OFFSET_X, GameConstants::GRID_OFFSET_Y);
    sf::Vector2f m_cellSize = sf::Vector2f(GameConstants::GRID_CELL_WIDTH, GameConstants::GRID_CELL_HEIGHT);

    std::unordered_set<sf::Vector2i, Vector2iHash> m_blockedCells;
};