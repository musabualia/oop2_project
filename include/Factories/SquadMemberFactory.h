#pragma once
#include "Core/Constants.h"
#include "Entities/Base/SquadMember.h"
#include "Utils/ConfigLoader.h"
#include <memory>
#include <string>
#include <unordered_map>

class HeavyGunnerMember;
class SniperMember;
class ShieldBearerMember;
class DemolitionMember;

class SquadMemberFactory {
public:
    static SquadMemberFactory& getInstance();                    // Singleton

    void initialize();                                           // Initialize factory
    void loadConfigurations();                                   // Load configs

    std::unique_ptr<SquadMember> createSquadMember(GameTypes::SquadMemberType type) const; // Create by type
    std::unique_ptr<SquadMember> createSquadMember(const std::string& typeName) const;     // Create by name
    std::unique_ptr<SquadMember> createSquadMemberWithLevel(GameTypes::SquadMemberType type, int level) const; // Create with level
    std::unique_ptr<SquadMember> createSquadMemberAtPosition(GameTypes::SquadMemberType type, int lane, int gridX) const; // Create at position
    std::vector<std::unique_ptr<SquadMember>> createSquadMembers(GameTypes::SquadMemberType type, int count) const; // Batch create

    int getSquadMemberCost(GameTypes::SquadMemberType type) const;                 // Cost
    int getSquadMemberUpgradeCost(GameTypes::SquadMemberType type, int currentLevel) const; // Upgrade cost
    bool canAffordSquadMember(GameTypes::SquadMemberType type, int availableCoins) const; // Check affordability

    ConfigLoader::SquadMemberConfig getSquadMemberConfig(GameTypes::SquadMemberType type) const; // Get config
    ConfigLoader::SquadMemberConfig getSquadMemberConfig(const std::string& typeName) const;

    bool canCreateSquadMember(GameTypes::SquadMemberType type) const;             // Can create
    bool canCreateSquadMember(const std::string& typeName) const;
    bool canPlaceSquadMember(GameTypes::SquadMemberType type, int lane, int gridX) const;

    void reloadConfigurations();                                                  // Reload configs
    bool isConfigurationLoaded() const;                                           // Config loaded

    void setSquadMemberUnlocked(GameTypes::SquadMemberType type, bool unlocked);  // Unlock control
    bool isSquadMemberUnlocked(GameTypes::SquadMemberType type) const;            // Is unlocked

    int getSquadMemberTypeCount() const;                                          // Count types
    std::vector<GameTypes::SquadMemberType> getAvailableTypes() const;            // Available types
    std::vector<GameTypes::SquadMemberType> getUnlockedTypes() const;             // Unlocked types
    std::vector<std::string> getAvailableTypeNames() const;                       // Type names

private:
    SquadMemberFactory() = default;

    std::unordered_map<GameTypes::SquadMemberType, ConfigLoader::SquadMemberConfig> m_squadConfigs;
    std::unordered_map<std::string, GameTypes::SquadMemberType> m_nameToType;
    std::unordered_map<GameTypes::SquadMemberType, bool> m_unlockedTypes;
    bool m_configurationsLoaded = false;

    void setupNameMappings();
    void loadSquadMemberConfiguration(GameTypes::SquadMemberType type, const std::string& configName);

    std::string squadMemberTypeToString(GameTypes::SquadMemberType type) const;
    GameTypes::SquadMemberType stringToSquadMemberType(const std::string& name) const;

    void applyConfigToSquadMember(SquadMember* member, const ConfigLoader::SquadMemberConfig& config) const;
    void applyLevelUpgrades(SquadMember* member, int level) const;
};
