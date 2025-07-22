
#include "Commands/PlaceUnitCommand.h"
#include "Core/GameManager.h"
#include "Entities/Base/SquadMember.h"
#include "Managers/SquadMemberManager.h"
#include "Managers/RobotManager.h"
#include "UI/GridRenderer.h"
#include <sstream>
#include <iostream>
#include <stdexcept> 

PlaceUnitCommand::PlaceUnitCommand(GameManager* gameManager,
    GameTypes::SquadMemberType unitType,
    int lane,
    int gridX,
    GridRenderer* gridRenderer)
    : m_gameManager(gameManager)
    , m_unitType(unitType)
    , m_lane(lane)
    , m_gridX(gridX)
    , m_gridRenderer(gridRenderer)
    , m_gridPosition(gridX, lane)
    , m_placedUnit(nullptr)
    , m_costPaid(0) {
}

void PlaceUnitCommand::execute() {
    if (m_executed) {
        return;
    }

    if (!m_gameManager) {
        return;
    }

    if (!isValidPlacement()) {
        return;
    }

    if (!hasEnoughCoins()) {
        return;
    }

    m_costPaid = getUnitCost();

    if (m_gameManager->canPlaceUnit(m_unitType, m_lane, m_gridX)) {
        if (m_gameManager->spendCoins(m_costPaid)) {
            if (m_gameManager->placeUnit(m_unitType, m_lane, m_gridX)) {
                m_placedUnit = m_gameManager->getSquadMemberManager().getUnitAt(m_lane, m_gridX);

                if (m_placedUnit) {
                    if (m_gridRenderer) {
                        m_gridRenderer->blockCell(m_gridPosition);
                    }

                    m_executed = true;
                }
                else {
                    m_gameManager->addCoins(m_costPaid);
                    m_costPaid = 0;
                }
            }
            else {
                m_gameManager->addCoins(m_costPaid);
                m_costPaid = 0;
            }
        }
    }
}

void PlaceUnitCommand::undo() {
    if (!m_executed || !m_placedUnit || !m_gameManager) {
        return;
    }

    if (isUnitInCombat()) {
        throw std::runtime_error("Cannot undo: Unit is in combat");
    }

    sf::Vector2f unitPosition = m_placedUnit->getPosition();
    m_placedUnit->setActive(false);

    auto& squadManager = m_gameManager->getSquadMemberManager();
    bool removed = squadManager.removeUnit(m_lane, m_gridX);

    if (removed) {
        if (m_gridRenderer) {
            m_gridRenderer->unblockCell(m_gridPosition);
        }

        m_gameManager->addCoins(m_costPaid);
        m_placedUnit = nullptr;
        m_executed = false;
    }
    else {
        if (m_placedUnit) {
            m_placedUnit->setActive(true);
        }
        throw std::runtime_error("Cannot undo: Unit removal failed");
    }
}

bool PlaceUnitCommand::canUndo() const {
    return m_executed &&
        m_placedUnit != nullptr &&
        m_gameManager != nullptr &&
        !isUnitInCombat();
}

bool PlaceUnitCommand::isUnitInCombat() const {
    if (!m_placedUnit) return false;

    // Validate before accessing
    if (!isUnitStillValid()) return true;

    if (m_placedUnit->isDestroyed()) return true;

    bool hasBeenDamaged = m_placedUnit->getHealth() < m_placedUnit->getMaxHealth();
    bool hasTarget = m_placedUnit->getCurrentTarget() != nullptr;

    if (m_gameManager) {
        auto& robotManager = m_gameManager->getRobotManager();
        const auto& robots = robotManager.getRobots();

        for (const auto& robot : robots) {
            if (robot && robot->isActive() && !robot->isDead()) {
                if (robot->isAttackingSquadMember() &&
                    robot->getTargetSquadMember() == m_placedUnit) {
                    return true; // Robot is actively attacking this unit
                }
            }
        }
    }

    return hasBeenDamaged || hasTarget;
}

std::string PlaceUnitCommand::getDescription() const {
    std::ostringstream desc;
    desc << "Place " << unitTypeToString(m_unitType)
        << " at (" << m_gridX << "," << m_lane << ")";
    return desc.str();
}

bool PlaceUnitCommand::isValidPlacement() const {
    if (!m_gameManager) return false;

    if (m_lane < 0 || m_lane >= GameConstants::GRID_ROWS) return false;
    if (m_gridX < 0 || m_gridX >= GameConstants::GRID_COLUMNS) return false;

    return m_gameManager->canPlaceUnit(m_unitType, m_lane, m_gridX);
}

bool PlaceUnitCommand::hasEnoughCoins() const {
    if (!m_gameManager) return false;

    int cost = getUnitCost();
    return m_gameManager->getCoins() >= cost;
}

std::string PlaceUnitCommand::unitTypeToString(GameTypes::SquadMemberType type) const {
    switch (type) {
    case GameTypes::SquadMemberType::HeavyGunner:
        return "Heavy Gunner";
    case GameTypes::SquadMemberType::Sniper:
        return "Sniper";
    case GameTypes::SquadMemberType::ShieldBearer:
        return "Shield Bearer";
    default:
        return "Unknown Unit";
    }
}

int PlaceUnitCommand::getUnitCost() const {
    if (!m_gameManager) return 0;

    switch (m_unitType) {
    case GameTypes::SquadMemberType::HeavyGunner: return 40;
    case GameTypes::SquadMemberType::Sniper: return 60;
    case GameTypes::SquadMemberType::ShieldBearer: return 50;
    default: return 0;
    }
}

bool PlaceUnitCommand::isUnitStillValid() const {
    if (!m_placedUnit || !m_gameManager) return false;

    // Check if unit still exists in the manager
    auto& squadManager = m_gameManager->getSquadMemberManager();
    const auto& entities = squadManager.getEntities();

    for (const auto& unit : entities) {
        if (unit.get() == m_placedUnit) {
            return true;  // Unit still exists
        }
    }
    return false;  // Unit was deleted
}