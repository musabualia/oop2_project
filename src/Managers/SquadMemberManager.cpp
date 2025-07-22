// ================================
// Squad Member Manager - Entity Manager Pattern for Player Unit Placement and Management
// ================================
#include "Managers/SquadMemberManager.h"
#include "Entities/Base/SquadMember.h"
#include "Entities/SquadMembers/HeavyGunnerMember.h"
#include "Entities/SquadMembers/SniperMember.h"
#include "Entities/SquadMembers/ShieldBearerMember.h"
#include "Core/GameManager.h"
#include "UI/HUD.h"
#include "UI/GridRenderer.h"
#include "Systems/AnimationSystem.h"
#include "Managers/RobotManager.h"
#include <iostream>
#include <cmath>

// Constructor - Initialize grid for unit placement
SquadMemberManager::SquadMemberManager() {
    initializeGrid();
}

// Main update loop - handles unit lifecycle and combat
void SquadMemberManager::update(float dt) {
    updateUnitTargeting(dt);    // Update unit target acquisition
    updateUnitCombat(dt);       // Handle combat calculations
    updateUnitAbilities(dt);    // Update special abilities

    if (!m_gameManager) return;

    // Get current robot list for targeting
    const auto& robots = m_gameManager->getRobotManager().getRobots();
    std::vector<SquadMember*> unitsToRemove;

    // Update all units and collect destroyed ones
    for (const auto& unit : m_entities) {
        if (!unit) continue;

        unit->setNearbyRobots(&robots);  // Provide robot list for targeting
        unit->update(dt);

        // Mark destroyed units for removal
        if (unit->isDestroyed() && !unit->isActive()) {
            unitsToRemove.push_back(unit.get());
        }
    }

    // Clean up robot targeting references before removing units
    for (SquadMember* unitToRemove : unitsToRemove) {
        if (!unitToRemove) continue;

        // Stop any robots targeting this unit
        for (const auto& robot : m_gameManager->getRobotManager().getRobots()) {
            if (!robot) continue;

            if (robot->isAttackingSquadMember() && robot->getTargetSquadMember() == unitToRemove) {
                robot->stopAttackingSquadMember();
            }
        }
    }

    // Clear grid positions and unblock cells
    for (SquadMember* unitToRemove : unitsToRemove) {
        if (!unitToRemove) continue;

        // Calculate grid position from world position
        sf::Vector2f unitPosition = unitToRemove->getPosition();
        sf::Vector2i gridPos;
        if (m_gridRenderer) {
            gridPos = m_gridRenderer->worldToGridPosition(unitPosition);
        }
        else {
            gridPos.x = static_cast<int>((unitPosition.x - GameConstants::GRID_OFFSET_X) / GameConstants::GRID_CELL_WIDTH);
            gridPos.y = static_cast<int>((unitPosition.y - GameConstants::GRID_OFFSET_Y) / GameConstants::GRID_CELL_HEIGHT);
        }

        // Clear grid and unblock for future placement
        clearGridPosition(gridPos.y, gridPos.x);
        if (m_gridRenderer) {
            m_gridRenderer->unblockCell(gridPos);
        }
    }

    // Remove units from entity list
    for (SquadMember* unitToRemove : unitsToRemove) {
        auto it = std::find_if(m_entities.begin(), m_entities.end(),
            [unitToRemove](const std::unique_ptr<SquadMember>& ptr) {
                return ptr.get() == unitToRemove;
            });

        if (it != m_entities.end()) {
            if (m_hud) notifyUnitSold(unitToRemove);  // Notify UI
            m_entities.erase(it);
            ++m_totalUnitsSold;
        }
    }
}

// Render all active units
void SquadMemberManager::draw(sf::RenderWindow& window) const {
    for (const auto& unit : m_entities) {
        unit->draw(window);
    }
}

// Check if unit can be placed at specific grid position
bool SquadMemberManager::canPlaceUnit(GameTypes::SquadMemberType type, int lane, int gridX) const {
    return isValidPosition(lane, gridX) && !isPositionOccupied(lane, gridX);
}

// Place unit using Factory Pattern
SquadMember* SquadMemberManager::placeUnit(GameTypes::SquadMemberType type, int lane, int gridX) {
    if (!canPlaceUnit(type, lane, gridX)) return nullptr;

    // Create unit using Factory Pattern
    std::unique_ptr<SquadMember> unit;
    switch (type) {
    case GameTypes::SquadMemberType::HeavyGunner:
        unit = std::make_unique<HeavyGunnerMember>();
        break;
    case GameTypes::SquadMemberType::Sniper:
        unit = std::make_unique<SniperMember>();
        break;
    case GameTypes::SquadMemberType::ShieldBearer:
        unit = std::make_unique<ShieldBearerMember>();
        break;
    default:
        std::cerr << "❌ ERROR: Unknown squad member type: " << static_cast<int>(type) << std::endl;
        return nullptr;
    }

    if (!unit) {
        std::cerr << "❌ ERROR: Failed to create squad member of type: " << static_cast<int>(type) << std::endl;
        return nullptr;
    }

    // Calculate world position from grid coordinates
    sf::Vector2f worldPos = m_gridRenderer ?
        m_gridRenderer->gridToWorldPosition(gridX, lane) :
        sf::Vector2f(gridX * GameConstants::GRID_CELL_WIDTH, lane * GameConstants::GRID_CELL_HEIGHT);

    // Center unit in grid cell
    worldPos += sf::Vector2f(GameConstants::GRID_CELL_WIDTH / 2.f, GameConstants::GRID_CELL_HEIGHT / 2.f);
    unit->setPosition(worldPos);

    // Add to management systems
    SquadMember* unitPtr = unit.get();
    m_entities.push_back(std::move(unit));
    setGridPosition(lane, gridX, unitPtr);
    ++m_totalUnitsPlaced;

    // Notify UI of placement
    if (m_hud) notifyUnitPlaced(unitPtr);

    return unitPtr;
}

// Remove unit by grid position
bool SquadMemberManager::removeUnit(int lane, int gridX) {
    SquadMember* unit = getUnitAt(lane, gridX);
    return unit ? removeUnit(unit) : false;
}

// Remove specific unit instance
bool SquadMemberManager::removeUnit(SquadMember* unit) {
    if (!unit) return false;

    auto it = std::find_if(m_entities.begin(), m_entities.end(),
        [unit](const std::unique_ptr<SquadMember>& ptr) { return ptr.get() == unit; });

    if (it != m_entities.end()) {
        // Calculate and clear grid position
        sf::Vector2f unitPosition = unit->getPosition();
        sf::Vector2i gridPos;
        if (m_gridRenderer) {
            gridPos = m_gridRenderer->worldToGridPosition(unitPosition);
        }
        else {
            gridPos.x = static_cast<int>((unitPosition.x - GameConstants::GRID_OFFSET_X) / GameConstants::GRID_CELL_WIDTH);
            gridPos.y = static_cast<int>((unitPosition.y - GameConstants::GRID_OFFSET_Y) / GameConstants::GRID_CELL_HEIGHT);
        }

        // Clean up grid and UI
        clearGridPosition(gridPos.y, gridPos.x);
        if (m_gridRenderer) m_gridRenderer->unblockCell(gridPos);
        if (m_hud) notifyUnitSold(unit);

        // Remove from entity list
        m_entities.erase(it);
        ++m_totalUnitsSold;
        return true;
    }
    return false;
}

// Range-based queries for robot detection system
std::vector<SquadMember*> SquadMemberManager::getSquadMembersInRange(const sf::Vector2f& center, float radius) const {
    return getUnitsInRange(center, radius);
}

std::vector<SquadMember*> SquadMemberManager::getActiveSquadMembersInRange(const sf::Vector2f& center, float radius) const {
    std::vector<SquadMember*> result;
    float radiusSquared = radius * radius;  // Avoid sqrt in loop

    for (const auto& unit : m_entities) {
        if (!unit || !unit->isActive() || unit->isDestroyed()) continue;

        // Distance check using squared distance for performance
        sf::Vector2f diff = unit->getPosition() - center;
        if ((diff.x * diff.x + diff.y * diff.y) <= radiusSquared) {
            result.push_back(unit.get());
        }
    }
    return result;
}

// Find closest squad member to position
SquadMember* SquadMemberManager::getClosestSquadMemberTo(const sf::Vector2f& position, float maxDistance) const {
    SquadMember* closest = nullptr;
    float closestDistSq = (maxDistance > 0) ? maxDistance * maxDistance : std::numeric_limits<float>::max();

    for (const auto& unit : m_entities) {
        if (!unit || !unit->isActive() || unit->isDestroyed()) continue;

        sf::Vector2f diff = unit->getPosition() - position;
        float distSq = diff.x * diff.x + diff.y * diff.y;

        if (distSq < closestDistSq) {
            closestDistSq = distSq;
            closest = unit.get();
        }
    }
    return closest;
}

// Check if any squad members exist in range
bool SquadMemberManager::hasSquadMembersInRange(const sf::Vector2f& center, float radius) const {
    float radiusSquared = radius * radius;

    for (const auto& unit : m_entities) {
        if (!unit || !unit->isActive() || unit->isDestroyed()) continue;
        sf::Vector2f diff = unit->getPosition() - center;
        if ((diff.x * diff.x + diff.y * diff.y) <= radiusSquared) {
            return true;
        }
    }
    return false;
}

// Count squad members in range
int SquadMemberManager::getSquadMemberCountInRange(const sf::Vector2f& center, float radius) const {
    int count = 0;
    float radiusSquared = radius * radius;

    for (const auto& unit : m_entities) {
        if (!unit || !unit->isActive() || unit->isDestroyed()) continue;
        sf::Vector2f diff = unit->getPosition() - center;
        if ((diff.x * diff.x + diff.y * diff.y) <= radiusSquared) {
            count++;
        }
    }
    return count;
}

// General range query for units
std::vector<SquadMember*> SquadMemberManager::getUnitsInRange(const sf::Vector2f& center, float radius) const {
    std::vector<SquadMember*> result;
    float radiusSquared = radius * radius;

    for (const auto& unit : m_entities) {
        if (!unit || !unit->isActive()) continue;
        sf::Vector2f diff = unit->getPosition() - center;
        if ((diff.x * diff.x + diff.y * diff.y) <= radiusSquared) {
            result.push_back(unit.get());
        }
    }
    return result;
}

// Find closest unit (including inactive ones)
SquadMember* SquadMemberManager::getClosestUnitTo(const sf::Vector2f& position) const {
    SquadMember* closest = nullptr;
    float closestDistSq = std::numeric_limits<float>::max();

    for (const auto& unit : m_entities) {
        if (!unit || !unit->isActive()) continue;
        sf::Vector2f diff = unit->getPosition() - position;
        float distSq = diff.x * diff.x + diff.y * diff.y;

        if (distSq < closestDistSq) {
            closestDistSq = distSq;
            closest = unit.get();
        }
    }
    return closest;
}

// Grid Management Methods

// Initialize grid to null pointers
void SquadMemberManager::initializeGrid() {
    for (auto& row : m_grid) {
        row.fill(nullptr);
    }
}

// Check if grid position is valid
bool SquadMemberManager::isValidPosition(int lane, int gridX) const {
    return StaticObject::isValidGridPosition(gridX, lane);
}

// Check if position is occupied by a unit
bool SquadMemberManager::isPositionOccupied(int lane, int gridX) const {
    return isValidPosition(lane, gridX) && m_grid[lane][gridX] != nullptr;
}

// Set unit at grid position
void SquadMemberManager::setGridPosition(int lane, int gridX, SquadMember* unit) {
    if (isValidPosition(lane, gridX)) {
        m_grid[lane][gridX] = unit;
    }
}

// Clear grid position
void SquadMemberManager::clearGridPosition(int lane, int gridX) {
    if (isValidPosition(lane, gridX)) {
        m_grid[lane][gridX] = nullptr;
    }
}

// Get unit at specific grid position
SquadMember* SquadMemberManager::getUnitAt(int lane, int gridX) const {
    return isValidPosition(lane, gridX) ? m_grid[lane][gridX] : nullptr;
}

// Get total number of units
int SquadMemberManager::getTotalUnitCount() const {
    return static_cast<int>(m_entities.size());
}

// UI Integration Methods

// Connect to HUD for notifications
void SquadMemberManager::setHUD(HUD* hud) {
    m_hud = hud;
}

// Connect to game manager
void SquadMemberManager::setGameManager(GameManager* gameManager) {
    m_gameManager = gameManager;
}

// Connect to grid renderer for visual feedback
void SquadMemberManager::setGridRenderer(GridRenderer* gridRenderer) {
    m_gridRenderer = gridRenderer;
}

// UI notification methods
void SquadMemberManager::notifyUnitPlaced(SquadMember* unit) {
    if (m_hud) m_hud->notifyUnitPlaced(unit);
}

void SquadMemberManager::notifyUnitUpgraded(SquadMember* unit) {
    if (m_hud) m_hud->notifyUnitUpgraded(unit);
}

void SquadMemberManager::notifyUnitSold(SquadMember* unit) {
    if (m_hud) m_hud->notifyUnitSold(unit);
}

void SquadMemberManager::notifyUnitSelected(SquadMember* unit) {
    if (m_hud) m_hud->notifyUnitSelected(unit);
}

// Update methods (can be expanded for specific combat logic)
void SquadMemberManager::updateUnitCombat(float dt) {
    // Future expansion: Handle unit combat calculations
}

void SquadMemberManager::updateUnitTargeting(float dt) {
    // Future expansion: Handle advanced targeting algorithms
}

void SquadMemberManager::updateUnitAbilities(float dt) {
    // Future expansion: Handle special unit abilities
}