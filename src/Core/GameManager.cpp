// ================================
// Game Manager - Central Controller for Game State and Entity Management
// ================================
#include "Core/GameManager.h"
#include "Factories/SquadMemberFactory.h"
#include "Factories/RobotFactory.h"
#include "Core/Constants.h"
#include "Utils/ConfigLoader.h"
#include "Physics/PhysicsWorld.h"
#include "Managers/RobotManager.h"
#include "Managers/SquadMemberManager.h"
#include "Managers/ProjectileManager.h"
#include "Managers/WaveManager.h"
#include "UI/HUD.h"
#include "Systems/EventSystem.h"
#include "Entities/Base/Collectible.h"
#include "Factories/CollectibleFactory.h"

#include <iostream>

// ================================
// Constructor - Initialize all game systems and managers
// ================================
GameManager::GameManager() {
    // Initialize factories for creating game entities
    RobotFactory::getInstance().initialize();
    CollectibleFactory::getInstance().initialize();

    // Set up physics simulation system
    initializePhysics();

    // Connect projectile system to physics world
    ProjectileManager::getInstance().setPhysicsWorld(m_physicsWorld.get());

    // Create all manager systems using smart pointers
    m_robotManager = std::make_unique<RobotManager>();
    m_robotManager->setGameManager(this);
    m_squadMemberManager = std::make_unique<SquadMemberManager>();
    m_waveManager = std::make_unique<WaveManager>();
    m_hud = std::make_unique<HUD>();
    m_collectibleManager = std::make_unique<EntityManager<Collectible>>();

    // Connect managers to each other for cross-communication
    m_hud->setGameManager(this);
    m_squadMemberManager->setHUD(m_hud.get());
    m_squadMemberManager->setGameManager(this);
    m_waveManager->setRobotManager(m_robotManager.get());
    m_robotManager->setSquadMemberManager(m_squadMemberManager.get());

    // Subscribe to events using Observer pattern
    EventSystem::getInstance().subscribe<RobotDestroyedEvent>([this](const RobotDestroyedEvent& e) {
        this->handleRobotDestroyed(e);
        });

    EventSystem::getInstance().subscribe<BombExplosionEvent>([this](const BombExplosionEvent& e) {
        this->handleBombExplosion(e);
        });

    EventSystem::getInstance().subscribe<CoinDropEvent>([this](const CoinDropEvent& e) {
        this->handleCoinDrop(e.position, e.robotRewardValue);
        });

    EventSystem::getInstance().subscribe<HealthPackDropEvent>([this](const HealthPackDropEvent& e) {
        this->handleHealthPackDrop(e.position);
        });
}

// ================================
// Destructor - Clean shutdown of all systems
// ================================
GameManager::~GameManager() {
    // Stop any ongoing robot attacks before cleanup
    if (m_robotManager && m_squadMemberManager) {
        for (const auto& robot : m_robotManager->getRobots()) {
            if (robot && robot->isAttackingSquadMember()) {
                robot->stopAttackingSquadMember();
            }
        }
    }

    // Clean up managers in proper order to avoid dangling pointers
    if (m_squadMemberManager) {
        m_squadMemberManager->clear();
        m_squadMemberManager.reset();
    }

    if (m_robotManager) {
        m_robotManager->clear();
        m_robotManager.reset();
    }

    // Clear singleton managers
    ProjectileManager::getInstance().clear();

    if (m_collectibleManager) {
        m_collectibleManager->clear();
        m_collectibleManager.reset();
    }

    // Reset remaining managers
    m_waveManager.reset();
    m_hud.reset();
    m_physicsWorld.reset();
}

// ================================
// Main Update Loop - Updates all game systems each frame
// ================================
void GameManager::update(float dt) {
    if (m_paused) return;  // Skip updates when game is paused

    m_elapsedTime += dt;

    // Step physics simulation forward
    if (m_physicsWorld) m_physicsWorld->step(dt);

    // Update all game managers
    updateManagers(dt);

    // Update and clean up collectibles (coins, health packs)
    if (m_collectibleManager) {
        m_collectibleManager->update(dt);
        m_collectibleManager->removeInactive();
    }

    // Handle temporary effects and check win/lose conditions
    updateGlobalEffects(dt);
    checkGameEndConditions();
}

// ================================
// Render - Draw all game elements to screen
// ================================
void GameManager::render(sf::RenderWindow& window) {
    m_squadMemberManager->draw(window);    // Player units
    m_robotManager->draw(window);          // Enemy robots
    if (m_collectibleManager) m_collectibleManager->draw(window);  // Coins/items
    ProjectileManager::getInstance().draw(window);  // Bullets/projectiles
    m_hud->draw(window);                   // User interface
}

// ================================
// Event Handling - Process input events
// ================================
void GameManager::handleEvent(const sf::Event& event) {
    m_hud->handleEvent(event);
}

// ================================
// Game State Management
// ================================
void GameManager::reset() {
    // Clear all entities
    m_squadMemberManager->clear();
    m_robotManager->clear();
    ProjectileManager::getInstance().clear();

    // Reset game state to initial values
    m_coins = GameConstants::Economy::getStartingCoinsForLevel(m_currentLevel);
    m_baseHealth = GameConstants::BASE_HEALTH;
    m_gameOver = false;
    m_victory = false;
    m_paused = false;
    m_elapsedTime = 0.0f;
    m_healthPackCount = 1;

    // Update UI with reset values
    m_hud->updateCoins(m_coins);
    m_hud->updateBaseHealth(m_baseHealth, GameConstants::BASE_HEALTH);
    m_hud->updateHealthPackCount(m_healthPackCount);
}

// Start new game with level-based starting resources
void GameManager::startNewGame() {
    reset();

    // Calculate coins based on current level (harder levels get more starting coins)
    m_coins = GameConstants::Economy::getStartingCoinsForLevel(m_currentLevel);
    m_healthPackCount = 1;

    // Initialize UI displays
    m_hud->updateCoins(m_coins);
    m_hud->updateBaseHealth(m_baseHealth, GameConstants::BASE_HEALTH);
    m_hud->updateScore(0);
    m_hud->updateWaveInfo(1, 10);
    m_hud->updateHealthPackCount(m_healthPackCount);

    // Start wave countdown
    m_waveManager->showWaveCountdown(5.0f);
}

void GameManager::pauseGame() { m_paused = true; }
void GameManager::resumeGame() { m_paused = false; }

// ================================
// Unit Placement System
// ================================
bool GameManager::placeUnit(GameTypes::SquadMemberType type, int lane, int gridX) {
    if (!canPlaceUnit(type, lane, gridX)) return false;

    SquadMember* unit = m_squadMemberManager->placeUnit(type, lane, gridX);
    if (unit) {
        m_hud->updateCoins(m_coins);  // Update UI after successful placement
        return true;
    }
    std::cerr << "Failed to place unit\n";
    return false;
}

bool GameManager::canPlaceUnit(GameTypes::SquadMemberType type, int lane, int gridX) const {
    return m_squadMemberManager && m_squadMemberManager->canPlaceUnit(type, lane, gridX);
}

// ================================
// Resource Management - Economy System
// ================================
int GameManager::getCoins() const { return m_coins; }

void GameManager::addCoins(int amount) {
    m_coins += amount;
    m_hud->updateCoins(m_coins);
}

bool GameManager::spendCoins(int cost) {
    if (m_coins < cost) return false;
    m_coins -= cost;
    m_hud->updateCoins(m_coins);
    return true;
}

// Health pack management for healing units
int GameManager::getHealthPackCount() const { return m_healthPackCount; }

void GameManager::addHealthPack(int count) {
    m_healthPackCount += count;
    m_hud->updateHealthPackCount(m_healthPackCount);
}

bool GameManager::useHealthPack() {
    if (m_healthPackCount <= 0) return false;
    m_healthPackCount--;
    m_hud->updateHealthPackCount(m_healthPackCount);
    return true;
}

void GameManager::setHealthPackCount(int count) {
    m_healthPackCount = std::max(0, count);
    m_hud->updateHealthPackCount(m_healthPackCount);
}

// ================================
// Getters for Game State
// ================================
bool GameManager::isGameOver() const { return m_gameOver; }
bool GameManager::isVictory() const { return m_victory; }
bool GameManager::isPaused() const { return m_paused; }
int GameManager::getCurrentWave() const { return m_waveManager->getCurrentWave(); }

PhysicsWorld* GameManager::getPhysicsWorld() const {
    return m_physicsWorld.get();
}

// Manager access methods
RobotManager& GameManager::getRobotManager() { return *m_robotManager; }
SquadMemberManager& GameManager::getSquadMemberManager() { return *m_squadMemberManager; }
HUD& GameManager::getHUD() { return *m_hud; }
WaveManager& GameManager::getWaveManager() { return *m_waveManager; }

// ================================
// Game Events - Handle critical game events
// ================================
void GameManager::onBaseReached() {
    if (m_victory) return;  // Don't process if already won

    m_baseHealth -= 10;  // Damage player base
    m_hud->updateBaseHealth(m_baseHealth, GameConstants::BASE_HEALTH);

    if (m_baseHealth <= 0) {
        m_gameOver = true;
        std::cout << "GAME OVER\n";
    }
}

// Handle robot destruction and drop creation
void GameManager::handleRobotDestroyed(const RobotDestroyedEvent& e) {
    if (m_collectibleManager && m_physicsWorld && m_physicsWorld->getWorld()) {
        // Create drops (coins/health packs) at robot's death location
        auto drops = CollectibleFactory::getInstance().createDropsFromRobot(e.robotType, e.position);
        for (auto& collectible : drops) {
            if (collectible) {
                try {
                    collectible->createPhysicsBody(m_physicsWorld->getWorld());
                    m_collectibleManager->addEntity(std::move(collectible));
                }
                catch (const std::exception& ex) {
                    std::cerr << "Error placing drop: " << ex.what() << std::endl;
                }
            }
        }
    }
}

// Add coins based on robot reward value (not fixed amount)
void GameManager::handleCoinDrop(const sf::Vector2f& position, int robotReward) {
    addCoins(robotReward);
}

void GameManager::handleHealthPackDrop(const sf::Vector2f& position) {
    addHealthPack(1);
}

// Handle bomb explosion damage to nearby robots
void GameManager::handleBombExplosion(const BombExplosionEvent& event) {
    auto robots = m_robotManager->getRobotsInRange(event.position, event.explosionRadius);
    for (Robot* robot : robots) {
        if (robot && robot->isActive() && !robot->isDead()) {
            robot->takeDamage(event.damage);
        }
    }
}

// ================================
// Private Helper Methods
// ================================
void GameManager::initializePhysics() {
    m_physicsWorld = std::make_unique<PhysicsWorld>();
    m_physicsWorld->initialize();
}

void GameManager::updateManagers(float dt) {
    m_robotManager->update(dt);
    m_squadMemberManager->update(dt);
    ProjectileManager::getInstance().update(dt);
    m_hud->update(dt);
    m_waveManager->update(dt);
}

void GameManager::updateGlobalEffects(float dt) {
    // Handle temporary damage multiplier effects
    if (m_globalBoost.active) {
        m_globalBoost.duration -= dt;
        if (m_globalBoost.duration <= 0) {
            m_globalBoost.active = false;
            m_globalBoost.damageMultiplier = 1.0f;
        }
    }
}

void GameManager::checkGameEndConditions() {
    if (m_victory || m_gameOver) return;

    // Check victory condition - all waves completed
    if (m_waveManager->areAllWavesCompleted()) {
        m_victory = true;
        std::cout << "VICTORY\n";
        return;
    }

    // Check defeat condition - base destroyed
    if (m_baseHealth <= 0) {
        m_gameOver = true;
        std::cout << "GAME OVER\n";
    }
}

// Level management for progressive difficulty
void GameManager::setCurrentLevel(int level) {
    m_currentLevel = level;
    if (m_waveManager) m_waveManager->setCurrentLevel(level);
}

int GameManager::getCurrentLevel() const { return m_currentLevel; }
float GameManager::getElapsedTime() const { return m_elapsedTime; }
int GameManager::getScore() const { return 0; }
int GameManager::getWavesCompleted() const {
    return m_waveManager ? m_waveManager->areAllWavesCompleted() : 0;
}