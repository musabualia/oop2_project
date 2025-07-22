// ================================
// include/States/PlayState.h - Game Play State
// Handles the main gameplay including unit placement, combat, and UI
// ================================
#pragma once

#include "States/IState.h"
#include "Core/Constants.h"
#include "Core/Timer.h"
#include "Commands/CommandManager.h"
#include "Entities/Projectiles/Bomb.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class GameManager;
class Game;
class StateMachine;
class GridRenderer;

class PlayState : public IState {
public:
    PlayState(Game* game, StateMachine* machine, int level = 1);
    virtual ~PlayState() = default;

    // Core state interface - handles events, updates game logic, and renders
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

    // State lifecycle - setup and cleanup
    void onEnter() override;
    void onExit() override;
    void onPause() override;
    void onResume() override;
    bool isPaused() const override;

private:
    // Core game systems - main managers for gameplay
    std::unique_ptr<GameManager> m_gameManager;      // Manages overall game state
    std::unique_ptr<GridRenderer> m_gridRenderer;    // Renders placement grid
    std::unique_ptr<CommandManager> m_commandManager; // Handles undo/redo system

    // External references
    Game* m_game;
    StateMachine* m_machine;

    // Game state variables
    int m_currentLevel = 1;
    bool m_paused = false;
    bool m_initialized = false;

    // Message system for user feedback
    bool m_showNotEnoughCoinsMessage = false;
    Timer m_notEnoughCoinsTimer;
    sf::Text m_notEnoughCoinsText;

    // Hover-based locked message system for sniper
    bool m_isHoveringLockedSniper = false;           // Track mouse hover state
    float m_lockedMessageAlpha = 0.0f;               // Current message transparency
    sf::Texture m_lockedMessageTexture;
    static constexpr float FADE_SPEED = 512.0f;      // Fade speed (alpha per second)

    // UI Elements - textures and positioning
    std::vector<sf::Texture> m_unitTextures;         // Unit selection icons
    std::vector<int> m_unitCosts;                    // Cost of each unit type
    std::vector<sf::FloatRect> m_unitIconBounds;     // Click areas for units

    sf::FloatRect m_bombIconBounds;
    sf::Texture m_coinUITexture;
    sf::Sprite m_coinUISprite;

    // Command system UI
    sf::Texture m_undoTexture;
    sf::Texture m_redoTexture;
    sf::FloatRect m_undoIconBounds;
    sf::FloatRect m_redoIconBounds;

    // Level display
    sf::Texture m_levelSelectTexture;
    sf::Sprite m_levelSelectSprite;
    sf::FloatRect m_levelSelectBounds;

    // HealthPack system
    sf::Texture m_healthPackTexture;
    sf::FloatRect m_healthPackIconBounds;
    bool m_healthPackMode = false;

    // Game objects
    std::vector<Bomb> m_bombs;                       // Active bombs in game

    // Input state - tracks what player is trying to place
    GameTypes::SquadMemberType m_selectedUnitType = GameTypes::SquadMemberType::HeavyGunner;
    bool m_bombMode = false;

    // Event handling methods - process user input
    void handleMouseEvents(const sf::Event& event);
    void handleMouseHover(const sf::Vector2f& mousePos);  // NEW: Handle hover detection
    void handleObjectPlacement(const sf::Vector2f& mousePos);
    void handleBombPlacement(const sf::Vector2f& mousePos);
    void handleSquadMemberPlacement(const sf::Vector2f& mousePos);
    void handleHealthPackUsage(const sf::Vector2f& mousePos);

    // Game control methods
    void checkGameEndConditions();                   // Check for win/lose conditions
    void startNewGame();                            // Initialize new game session

    // Rendering methods - draw all UI and game elements
    void renderBackground(sf::RenderWindow& window);
    void renderGameContent(sf::RenderWindow& window);
    void renderUI(sf::RenderWindow& window);
    void renderUnitSelectionIcons(sf::RenderWindow& window);
    void renderSpecialIcons(sf::RenderWindow& window);
    void renderHealthPackIcon(sf::RenderWindow& window);
    void renderCoinDisplay(sf::RenderWindow& window);
    void renderCommandIcons(sf::RenderWindow& window);
    void renderLevelSelect(sf::RenderWindow& window);
    void renderLockedMessage(sf::RenderWindow& window);

    // Helper methods
    float calculateMessageAlpha(float elapsedTime) const; // For coin message fade effect
};