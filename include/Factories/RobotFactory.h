#pragma once
#include "Core/Constants.h"
#include "Entities/Base/Robot.h"
#include "Utils/ConfigLoader.h"
#include <memory>
#include <string>
#include <unordered_map>

class BasicRobot;
class FireRobot;
class RunnerRobot;
class TankRobot;
class StealthRobot;

class RobotFactory {
public:
    static RobotFactory& getInstance();                     // Singleton instance

    void initialize();                                      // Initialize factory
    void loadConfigurations();                              // Load robot configs

    std::unique_ptr<Robot> createRobot(GameTypes::RobotType type) const;    // Create robot by type
    std::unique_ptr<Robot> createRobot(const std::string& typeName) const;  // Create robot by name

    std::vector<std::unique_ptr<Robot>> createRobots(GameTypes::RobotType type, int count) const;  // Batch create
    std::vector<std::unique_ptr<Robot>> createMixedRobots(const std::vector<GameTypes::RobotType>& types) const; // Mixed create

    std::unique_ptr<Robot> createRandomRobot() const;       // Create random robot
    std::unique_ptr<Robot> createRandomRobotForWave(int waveNumber) const; // Random for wave

    ConfigLoader::RobotConfig getRobotConfig(GameTypes::RobotType type) const;  // Get config by type
    ConfigLoader::RobotConfig getRobotConfig(const std::string& typeName) const;// Get config by name

    bool canCreateRobot(GameTypes::RobotType type) const;   // Validate robot type
    bool canCreateRobot(const std::string& typeName) const; // Validate by name

    void reloadConfigurations();                            // Reload configs
    bool isConfigurationLoaded() const;                     // Check if configs loaded

    int getRobotTypeCount() const;                          // Count robot types
    std::vector<GameTypes::RobotType> getAvailableTypes() const;     // List of types
    std::vector<std::string> getAvailableTypeNames() const;          // List of type names

private:
    RobotFactory() = default;

    std::unordered_map<GameTypes::RobotType, ConfigLoader::RobotConfig> m_robotConfigs; // Configs
    std::unordered_map<std::string, GameTypes::RobotType> m_nameToType;                // Name map
    bool m_configurationsLoaded = false;

    void setupNameMappings();                      // Setup name mappings
    void loadRobotConfiguration(GameTypes::RobotType type, const std::string& configName); // Load config

    std::string robotTypeToString(GameTypes::RobotType type) const;   // Convert to string
    GameTypes::RobotType stringToRobotType(const std::string& name) const; // String to type

    void applyConfigToRobot(Robot* robot, const ConfigLoader::RobotConfig& config) const; // Apply config
};
