// ================================
// include/Utils/ConfigLoader.h
// ================================
#pragma once
#include "Core/Constants.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <vector>

class ConfigLoader {
public:
    // Singleton access
    static ConfigLoader& getInstance();

    // Initialize all configurations at startup
    bool initializeAllConfigs();
    bool isInitialized() const { return m_initialized; }

    // File operations
    bool loadFromFile(const std::string& filename);
    void clearConfig();

    // Value getters with default fallbacks
    bool getBool(const std::string& section, const std::string& key, bool defaultValue = false) const;
    int getInt(const std::string& section, const std::string& key, int defaultValue = 0) const;
    float getFloat(const std::string& section, const std::string& key, float defaultValue = 0.0f) const;
    std::string getString(const std::string& section, const std::string& key, const std::string& defaultValue = "") const;

    // Check if value exists
    bool hasSection(const std::string& section) const;
    bool hasValue(const std::string& section, const std::string& key) const;

    // Convenient getters for specific config types
    int getUnitCost(GameTypes::SquadMemberType type) const;
    int getRobotReward(GameTypes::RobotType type) const;
    int getProjectileDamage(GameTypes::ProjectileType type) const;
    float getProjectileSpeed(GameTypes::ProjectileType type) const;

    // Collectibles configuration
    float getDropRate(GameTypes::RobotType robotType, const std::string& dropType) const;
    bool hasCollectiblesConfig() const;
    bool loadCollectiblesConfig();
    float getHealthPackHealPercentage() const;

    // Physics configuration getters
    float getPixelsPerMeter() const;
    float getGravityX() const;
    float getGravityY() const;
    int getVelocityIterations() const;
    int getPositionIterations() const;

    // Config structures
    struct RobotConfig {
        GameTypes::RobotType type;
        int health;
        float speed;
        int damage;
        int reward;
        float attackRange = 100.0f;
        int meleeAttackDamage = 25;
        float bulletCooldown = 0.0f;
    };

    struct SquadMemberConfig {
        GameTypes::SquadMemberType type;
        int cost;
        float range;
        int health;
    };

    struct ProjectileConfig {
        GameTypes::ProjectileType type;
        float speed;
        std::string texture;
        float scale;
        sf::Vector2f spawnOffset;
        float maxRange;
        int damage;
    };

    struct PhysicsConfig {
        float pixelsPerMeter;
        float gravityX;
        float gravityY;
        int velocityIterations;
        int positionIterations;

        // Robot physics
        float robotRadius;
        float robotMass;
        float robotFriction;

        // Squad member physics
        float squadWidth;
        float squadHeight;
        float squadMass;

        // Projectile physics
        float bulletRadius;
        float rocketRadius;
        float bulletMass;
    };

    struct GameConfig {
        unsigned int windowWidth;
        unsigned int windowHeight;
        std::string windowTitle;
        float masterVolume;
        float musicVolume;
        float sfxVolume;
        int initialCoins;
        int baseHealth;
        int numLanes;
    };

    // Load specific configurations
    RobotConfig loadRobotConfig(const std::string& robotName) const;
    SquadMemberConfig loadSquadMemberConfig(const std::string& unitName) const;
    ProjectileConfig loadProjectileConfig(const std::string& projectileName) const;
    PhysicsConfig loadPhysicsConfig() const;
    GameConfig loadGameConfig() const;

    // Validation methods
    bool validateAllConfigs() const;
    std::vector<std::string> getMissingConfigs() const;

    // Error handling
    std::string getLastError() const;
    bool hasErrors() const;

private:
    ConfigLoader() = default;
    ~ConfigLoader() = default;
    ConfigLoader(const ConfigLoader&) = delete;
    ConfigLoader& operator=(const ConfigLoader&) = delete;

    // Internal storage
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_config;
    mutable std::string m_lastError;
    bool m_initialized = false;

    // Track which configs are loaded
    std::vector<std::string> m_loadedConfigs;

    // Required config files
    static const std::vector<std::string> REQUIRED_CONFIG_FILES;

    // Helper methods
    bool parseINI(const std::string& content);
    std::string readFileContent(const std::string& filename) const;
    std::string trim(const std::string& str) const;
    bool stringToBool(const std::string& str) const;

    // Error handling
    void setError(const std::string& error) const;
    void clearError() const;
    void addLoadedConfig(const std::string& configName);
};