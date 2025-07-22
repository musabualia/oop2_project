// ================================
// Static Object - Base class for non-moving entities (squad members, buildings)
// ================================
#include "Core/StaticObject.h"
#include "Core/Constants.h"
#include "Physics/PhysicsUtils.h"

// ================================
// Lane Management - Set which row/lane the object occupies
// ================================
void StaticObject::setLane(int lane) {
    // Validate lane is within game grid bounds
    if (lane >= 0 && lane < GameConstants::GRID_ROWS) {
        m_lane = lane;

        // Calculate world position from grid coordinates
        sf::Vector2f worldPos = gridToWorldPosition(m_gridPosition.x, lane);
        // Center object in grid cell
        worldPos += sf::Vector2f(GameConstants::GRID_CELL_WIDTH / 2.f, GameConstants::GRID_CELL_HEIGHT / 2.f);
        setPosition(worldPos);
    }
}

int StaticObject::getLane() const {
    return m_lane;
}

// ================================
// Grid Position Management - Handle grid-based placement system
// ================================
void StaticObject::setGridPosition(int x, int y) {
    // Validate grid coordinates are within bounds
    if (isValidGridPosition(x, y)) {
        m_gridPosition.x = x;
        m_gridPosition.y = y;
        m_lane = y; // Lane corresponds to grid Y coordinate

        // Convert grid coordinates to world position
        sf::Vector2f worldPos = gridToWorldPosition(x, y);
        // Center object within the grid cell
        worldPos += sf::Vector2f(GameConstants::GRID_CELL_WIDTH / 2.f, GameConstants::GRID_CELL_HEIGHT / 2.f);
        setPosition(worldPos);

        m_isPlaced = true;  // Mark as successfully placed
    }
}

sf::Vector2i StaticObject::getGridPosition() const {
    return m_gridPosition;
}

// ================================
// Placement Validation - Check if object can be placed at location
// ================================
bool StaticObject::canBePlacedAt(int lane, int gridX) const {
    return isValidGridPosition(gridX, lane);
}

// Static method to validate grid coordinates
bool StaticObject::isValidGridPosition(int gridX, int lane) {
    return (gridX >= 0 && gridX < GameConstants::GRID_COLUMNS) &&
        (lane >= 0 && lane < GameConstants::GRID_ROWS);
}

// ================================
// Coordinate Conversion - Transform between grid and world coordinates
// ================================
sf::Vector2f StaticObject::gridToWorldPosition(int gridX, int lane) {
    // Calculate world position from grid coordinates using constants
    float x = GameConstants::GRID_OFFSET_X + gridX * GameConstants::GRID_CELL_WIDTH;
    float y = GameConstants::GRID_OFFSET_Y + lane * GameConstants::GRID_CELL_HEIGHT;
    return sf::Vector2f(x, y);
}

sf::Vector2i StaticObject::worldToGridPosition(const sf::Vector2f& worldPos) {
    // Convert world coordinates back to grid indices
    int gridX = static_cast<int>((worldPos.x - GameConstants::GRID_OFFSET_X) / GameConstants::GRID_CELL_WIDTH);
    int lane = static_cast<int>((worldPos.y - GameConstants::GRID_OFFSET_Y) / GameConstants::GRID_CELL_HEIGHT);
    return sf::Vector2i(gridX, lane);
}

// ================================
// Grid Alignment - Snap object to nearest grid cell
// ================================
void StaticObject::snapToGrid() {
    if (m_isPlaced) {
        // Calculate exact world position for current grid coordinates
        sf::Vector2f worldPos = gridToWorldPosition(m_gridPosition.x, m_gridPosition.y);
        // Center in grid cell
        worldPos += sf::Vector2f(GameConstants::GRID_CELL_WIDTH / 2.f, GameConstants::GRID_CELL_HEIGHT / 2.f);
        setPosition(worldPos);
    }
}