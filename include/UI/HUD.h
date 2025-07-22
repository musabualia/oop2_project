#pragma once
#include "Core/Constants.h"
#include "UI/Button.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <unordered_map>

class GameManager;
class SquadMember;

class HUD {
public:
    HUD();
    ~HUD() = default;

    // Core functionality
    void update(float dt);
    void draw(sf::RenderWindow& window) const;
    void handleEvent(const sf::Event& event);

    // Game manager integration
    void setGameManager(GameManager* gameManager);

    // Resource display updates
    void updateCoins(int coins);
    void updateBaseHealth(int health, int maxHealth);
    void updateScore(int score);
    void updateWaveInfo(int currentWave, int totalWaves);

    // ✅ NEW: HealthPack counter display
    void updateHealthPackCount(int count);

    // ADDED: Missing methods that UnitManager needs
    void updateUnitCount(int count);
    void notifyUnitPlaced(SquadMember* unit);
    void notifyUnitUpgraded(SquadMember* unit);
    void notifyUnitSold(SquadMember* unit);
    void notifyUnitSelected(SquadMember* unit);

    // Unit selection
    void setSelectedUnitType(GameTypes::SquadMemberType type);
    GameTypes::SquadMemberType getSelectedUnitType() const;

    // Unit visual feedback (moved from SquadMembers)
    void showUnitInfo(SquadMember* unit);
    void hideUnitInfo();
    void showUnitStats(SquadMember* unit);
    void showUpgradePreview(SquadMember* unit);
    void showRangePreview(GameTypes::SquadMemberType type, const sf::Vector2f& position);
    void hideRangePreview();

    // Unit range indicators (moved from SquadMember)
    void showRangeIndicator(SquadMember* unit, bool show);
    void showAllRangeIndicators(bool show);
    void updateRangeIndicator(SquadMember* unit);

    // Selection feedback (moved from SquadMember)
    void highlightSelectedUnit(SquadMember* unit);
    void clearUnitHighlight();
    void showPlacementGrid(bool show);
    void highlightAvailablePositions(GameTypes::SquadMemberType type);
    void clearHighlights();

    // Unit targeting visualization (moved from SquadMember)
    void showTargetingLine(SquadMember* unit, const sf::Vector2f& targetPos);
    void hideTargetingLine();
    void showAttackEffect(SquadMember* unit);

    // ✅ NEW: HealthPack usage visual feedback
    void showHealthPackUsage(SquadMember* target);

private:
    GameManager* m_gameManager = nullptr;

    // Essential UI elements
    sf::Text m_coinsText;
    sf::Text m_scoreText;
    sf::Text m_healthText;
    sf::Text m_waveText;

    // ✅ NEW: HealthPack counter text (for PlayState integration)
    sf::Text m_healthPackCountText;
    int m_currentHealthPackCount = 1;

    // Health bar
    sf::RectangleShape m_healthBarBackground;
    sf::RectangleShape m_healthBarFill;

    // Unit selection
    std::vector<std::unique_ptr<Button>> m_unitButtons;
    GameTypes::SquadMemberType m_selectedUnitType = GameTypes::SquadMemberType::HeavyGunner;

    // Unit visual feedback (moved from individual SquadMembers)
    sf::RectangleShape m_unitInfoPanel;
    sf::Text m_unitNameText;
    sf::Text m_unitStatsText;
    sf::Text m_unitLevelText;
    sf::CircleShape m_rangePreview;
    sf::RectangleShape m_unitHighlight;
    std::vector<sf::RectangleShape> m_gridCells;

    // Range indicators (moved from SquadMember)
    std::vector<sf::CircleShape> m_rangeIndicators;
    std::unordered_map<SquadMember*, sf::CircleShape> m_unitRangeIndicators;

    // Targeting visualization (moved from SquadMember)
    sf::Vertex m_targetingLine[2];
    bool m_showTargetingLine = false;
    sf::CircleShape m_attackEffect;

    bool m_showingUnitInfo = false;
    bool m_showingRangePreview = false;
    bool m_showingPlacementGrid = false;
    bool m_showRangeIndicators = false;
    SquadMember* m_selectedUnit = nullptr;

    // Helper methods
    void setupUI();
    void updateHealthBar();
    void updateUnitInfo(float dt);
    void updateVisualFeedback(float dt);
    void updateRangeIndicators(float dt);
    void onUnitButtonClicked(GameTypes::SquadMemberType type);
    void renderUnitInfo(sf::RenderWindow& window) const;
    void renderVisualFeedback(sf::RenderWindow& window) const;
    void renderRangeIndicators(sf::RenderWindow& window) const;
    void renderTargetingVisualization(sf::RenderWindow& window) const;
};