#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "Core/Constants.h"
#include "Managers/EntityManager.h"

class RobotManager;                      // Forward declarations
class SquadMemberManager;
class HUD;
class WaveManager;
class Robot;
class SquadMember;
class PhysicsWorld;
class Collectible;

struct BombExplosionEvent;
struct RobotDestroyedEvent;
struct CoinDropEvent;
struct HealthPackDropEvent;

class GameManager {
public:
    GameManager();                                       // Constructor
    ~GameManager();                                      // Destructor

    void update(float dt);                               // Update game logic
    void render(sf::RenderWindow& window);               // Render game elements
    void handleEvent(const sf::Event& event);            // Handle window/game events

    void reset();                                        // Reset game state
    void startNewGame();                                 // Start new game session
    void pauseGame();                                    // Pause game
    void resumeGame();                                   // Resume game

    PhysicsWorld* getPhysicsWorld() const;               // Get physics world reference

    RobotManager& getRobotManager();                     // Access RobotManager
    SquadMemberManager& getSquadMemberManager();         // Access SquadMemberManager
    HUD& getHUD();                                       // Access HUD
    WaveManager& getWaveManager();                       // Access WaveManager

    int getCoins() const;                                // Get current coin count
    void addCoins(int amount);                           // Add coins
    bool spendCoins(int cost);                           // Spend coins if enough

    int getHealthPackCount() const;                      // Get health pack count
    void addHealthPack(int count = 1);                   // Add health pack(s)
    bool useHealthPack();                                // Use a health pack if available
    void setHealthPackCount(int count);                  // Set health pack count directly

    bool isGameOver() const;                             // Check if game is over
    bool isVictory() const;                              // Check if player won
    bool isPaused() const;                               // Check if game is paused
    int getCurrentWave() const;                          // Get current wave number

    void setCurrentLevel(int level);                     // Set the current level
    int getCurrentLevel() const;                         // Get the current level

    bool canPlaceUnit(GameTypes::SquadMemberType unitType, int lane, int gridX) const;  // Check if unit can be placed
    bool placeUnit(GameTypes::SquadMemberType type, int lane, int gridX);               // Place a unit

    void onRobotDestroyed(Robot* robot);                 // Handle robot destroyed
    void onUnitDestroyed(SquadMember* unit);             // Handle unit destroyed
    void onWaveCompleted();                              // Handle wave completion
    void onBaseReached();                                // Handle when base is reached

    void handleRobotDestroyed(const RobotDestroyedEvent& event);    // Process robot destroyed event

    void handleCoinDrop(const sf::Vector2f& position, int robotReward);    // Handle coin drop
    void handleHealthPackDrop(const sf::Vector2f& position);              // Handle health pack drop

    float getElapsedTime() const;                        // Get elapsed game time
    int getScore() const;                                // Get current score
    int getWavesCompleted() const;                       // Get number of completed waves

private:
    std::unique_ptr<PhysicsWorld> m_physicsWorld;        // Physics simulation world
    std::unique_ptr<RobotManager> m_robotManager;        // Manages robots
    std::unique_ptr<SquadMemberManager> m_squadMemberManager;  // Manages squad members
    std::unique_ptr<HUD> m_hud;                          // Heads-up display manager
    std::unique_ptr<WaveManager> m_waveManager;          // Manages enemy waves
    std::unique_ptr<EntityManager<Collectible>> m_collectibleManager;  // Manages collectibles

    int m_coins = GameConstants::Economy::getStartingCoinsForLevel(1);  // Current coins
    int m_baseHealth = GameConstants::BASE_HEALTH;     // Player base health
    bool m_gameOver = false;                           // Is the game over
    bool m_victory = false;                            // Is it victory
    bool m_paused = false;                             // Is game paused
    float m_elapsedTime = 0.0f;                        // Elapsed time since start

    int m_currentLevel = 1;                            // Current game level
    int m_healthPackCount = 1;                         // Number of health packs

    struct GlobalEffect {                              // Struct for global effects
        float damageMultiplier = 1.0f;
        float duration = 0.0f;
        bool active = false;
    } m_globalBoost;                                   // Current global effect

    void initializePhysics();                          // Initialize physics world
    void updateManagers(float dt);                     // Update game managers
    void checkGameEndConditions();                     // Check win/lose conditions
    void updateGlobalEffects(float dt);                // Update global effect timers
    void handleBombExplosion(const BombExplosionEvent& event);   // Handle bomb explosions
};
