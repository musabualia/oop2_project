#pragma once
#include "ICommand.h"
#include "Core/Constants.h"
#include <SFML/Graphics.hpp>

class GameManager;                     // Forward declarations
class SquadMember;
class GridRenderer;

class PlaceUnitCommand : public ICommand {
public:
    PlaceUnitCommand(GameManager* gameManager,
        GameTypes::SquadMemberType unitType,
        int lane,
        int gridX,
        GridRenderer* gridRenderer);                       // Create command to place unit

    virtual ~PlaceUnitCommand() = default;                 // Default destructor

    void execute() override;                               // Place the unit on grid
    void undo() override;                                  // Undo unit placement
    bool canUndo() const override;                         // Check if undo is allowed
    std::string getDescription() const override;           // Return command description

    GameTypes::SquadMemberType getUnitType() const { return m_unitType; }     // Get unit type
    int getLane() const { return m_lane; }                                    // Get lane index
    int getGridX() const { return m_gridX; }                                  // Get grid X position
    SquadMember* getPlacedUnit() const { return m_placedUnit; }               // Get the placed unit
    int getCostPaid() const { return m_costPaid; }                            // Get cost spent for unit

    bool isValidPlacement() const;                    // Validate placement position
    bool hasEnoughCoins() const;                      // Check if player has enough coins

private:
    GameManager* m_gameManager;                       // Game manager reference
    GameTypes::SquadMemberType m_unitType;            // Type of unit to place
    int m_lane;                                       // Lane to place unit
    int m_gridX;                                      // Grid X coordinate
    GridRenderer* m_gridRenderer;                     // Grid renderer reference
    sf::Vector2i m_gridPosition;                      // Grid coordinates

    SquadMember* m_placedUnit = nullptr;              // Pointer to placed unit
    int m_costPaid = 0;                               // Coins paid to place unit

    bool isUnitInCombat() const;                      // Check if unit is in combat
    std::string unitTypeToString(GameTypes::SquadMemberType type) const;   // Convert unit type to string
    int getUnitCost() const;                          // Get unit cost
    bool isUnitStillValid() const;                    // Validate if unit still exists
};
