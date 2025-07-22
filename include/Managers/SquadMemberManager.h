#pragma once

#include "Managers/EntityManager.h"
#include "Entities/Base/SquadMember.h"
#include "Core/Constants.h"
#include "Core/Timer.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <array>
#include <vector>

class SquadMemberFactory;
class GameManager;
class HUD;
class GridRenderer;

class SquadMemberManager : public EntityManager<SquadMember> {
public:
    SquadMemberManager();
    virtual ~SquadMemberManager() = default;

    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;

    // Unit placement
    bool canPlaceUnit(GameTypes::SquadMemberType type, int lane, int gridX) const;
    SquadMember* placeUnit(GameTypes::SquadMemberType type, int lane, int gridX);
    bool removeUnit(int lane, int gridX);
    bool removeUnit(SquadMember* unit);

    // Grid management
    SquadMember* getUnitAt(int lane, int gridX) const;
    int getTotalUnitCount() const;
    bool isPositionOccupied(int lane, int gridX) const;
    bool isValidPosition(int lane, int gridX) const;

    // Query methods (EXISTING)
    std::vector<SquadMember*> getUnitsInLane(int lane) const;
    std::vector<SquadMember*> getUnitsInRange(const sf::Vector2f& center, float radius) const;
    std::vector<SquadMember*> getUnitsByType(GameTypes::SquadMemberType type) const;
    SquadMember* getClosestUnitTo(const sf::Vector2f& position) const;

    std::vector<SquadMember*> getSquadMembersInRange(const sf::Vector2f& center, float radius) const;
    std::vector<SquadMember*> getActiveSquadMembersInRange(const sf::Vector2f& center, float radius) const;
    SquadMember* getClosestSquadMemberTo(const sf::Vector2f& position, float maxDistance = -1.0f) const;

    // Query helpers for robot managers
    bool hasSquadMembersInRange(const sf::Vector2f& center, float radius) const;
    int getSquadMemberCountInRange(const sf::Vector2f& center, float radius) const;


    void setGridSize(int width, int height);
    sf::Vector2i getGridSize() const;

    // HUD and GameManager integration
    void setHUD(HUD* hud);
    void setGameManager(GameManager* gameManager);
    void notifyUnitPlaced(SquadMember* unit);
    void notifyUnitUpgraded(SquadMember* unit);
    void notifyUnitSold(SquadMember* unit);
    void notifyUnitSelected(SquadMember* unit);

    // Grid renderer integration
    void setGridRenderer(GridRenderer* gridRenderer);

private:
    SquadMemberFactory* m_squadMemberFactory = nullptr;
    GameManager* m_gameManager = nullptr;
    HUD* m_hud = nullptr;
    GridRenderer* m_gridRenderer = nullptr;

    std::array<std::array<SquadMember*, 10>, GameConstants::GRID_ROWS> m_grid = {};
    int m_gridWidth = 10;
    int m_gridHeight = GameConstants::GRID_ROWS;
    int m_maxUnitsPerLane = 8;

    SquadMember* m_selectedUnit = nullptr;
    Timer m_selectionTimer;

    int m_totalUnitsPlaced = 0;
    int m_totalUnitsSold = 0;
    int m_totalUpgrades = 0;

    void updateUnitCombat(float dt);
    void updateUnitTargeting(float dt);
    void updateUnitAbilities(float dt);
    void initializeGrid();

    void clearGridPosition(int lane, int gridX);
    void setGridPosition(int lane, int gridX, SquadMember* unit);
};