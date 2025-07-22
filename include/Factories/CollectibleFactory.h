#pragma once
#include "Core/Constants.h"
#include "Entities/Base/Collectible.h"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

class CollectibleFactory {
public:
    enum class DropOutcome { Nothing, Coin, HealthPack };         // Drop outcomes

    static CollectibleFactory& getInstance();                     // Singleton

    void initialize();                                            // Initialize

    std::unique_ptr<Collectible> createCollectible(GameTypes::CollectibleType type) const;      // Create collectible
    std::unique_ptr<Collectible> createCollectible(GameTypes::CollectibleType type, int value) const;
    std::unique_ptr<Collectible> createCoin(int value) const;     // Create coin
    std::unique_ptr<Collectible> createHealthPack(int healAmount = GameConstants::HEALTH_PACK_VALUE) const; // Health pack

    std::unique_ptr<Collectible> spawnCollectibleAt(GameTypes::CollectibleType type, const sf::Vector2f& position) const; // Spawn at position
    std::vector<std::unique_ptr<Collectible>> createDropsFromRobot(GameTypes::RobotType robotType, const sf::Vector2f& position) const; // Drops from robot

    void setRobotDropChance(GameTypes::RobotType robotType, float chance); // Set drop chance
    float getRobotDropChance(GameTypes::RobotType robotType) const;        // Get drop chance
    int getHealthPackHealPercentage() const;                              // Get health pack heal %

private:
    CollectibleFactory() = default;

    std::unordered_map<GameTypes::RobotType, float> m_robotDropChances;    // Drop chances
    bool m_configLoaded = false;

    void setupDefaultRobotDropChances();                                   // Setup default chances
    sf::Vector2f getRandomPositionNear(const sf::Vector2f& center, float spread) const; // Randomize position
    DropOutcome determineDropOutcome(GameTypes::RobotType robotType) const; // Determine drop
};
