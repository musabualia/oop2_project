// ================================
// src/Factories/SquadMemberFactory.cpp
// ================================
#include "Factories/SquadMemberFactory.h"
#include "Entities/SquadMembers/HeavyGunnerMember.h"
#include "Entities/SquadMembers/SniperMember.h"
#include "Entities/SquadMembers/ShieldBearerMember.h"
#include "Utils/ConfigLoader.h"
#include <iostream>

SquadMemberFactory& SquadMemberFactory::getInstance() {
    static SquadMemberFactory instance;
    return instance;
}

void SquadMemberFactory::initialize() {
    setupNameMappings();
    loadConfigurations();
}

void SquadMemberFactory::setupNameMappings() {
    m_nameToType["HeavyGunner"] = GameTypes::SquadMemberType::HeavyGunner;
    m_nameToType["Sniper"] = GameTypes::SquadMemberType::Sniper;
    m_nameToType["ShieldBearer"] = GameTypes::SquadMemberType::ShieldBearer;

    // Set all types as unlocked by default
    m_unlockedTypes[GameTypes::SquadMemberType::HeavyGunner] = true;
    m_unlockedTypes[GameTypes::SquadMemberType::Sniper] = true;
    m_unlockedTypes[GameTypes::SquadMemberType::ShieldBearer] = true;
}

void SquadMemberFactory::loadConfigurations() {
    loadSquadMemberConfiguration(GameTypes::SquadMemberType::HeavyGunner, "HeavyGunner");
    loadSquadMemberConfiguration(GameTypes::SquadMemberType::Sniper, "Sniper");
    loadSquadMemberConfiguration(GameTypes::SquadMemberType::ShieldBearer, "ShieldBearer");

    m_configurationsLoaded = true;
}

void SquadMemberFactory::loadSquadMemberConfiguration(GameTypes::SquadMemberType type, const std::string& configName) {
    try {
        auto config = ConfigLoader::getInstance().loadSquadMemberConfig(configName);
        m_squadConfigs[type] = config;
    }
    catch (const std::exception& e) {
        std::cerr << "❌ ERROR: Failed to load config for " << configName << ": " << e.what() << std::endl;
    }
}

std::unique_ptr<SquadMember> SquadMemberFactory::createSquadMember(GameTypes::SquadMemberType type) const {
    if (!canCreateSquadMember(type)) {
        return nullptr;
    }

    std::unique_ptr<SquadMember> member;

    switch (type) {
    case GameTypes::SquadMemberType::HeavyGunner:
        member = std::make_unique<HeavyGunnerMember>();
        break;
    case GameTypes::SquadMemberType::Sniper:
        member = std::make_unique<SniperMember>();
        break;
    case GameTypes::SquadMemberType::ShieldBearer:
        member = std::make_unique<ShieldBearerMember>();
        break;
    default:
        return nullptr;
    }

    // Apply configuration if available
    if (member) {
        auto configIt = m_squadConfigs.find(type);
        if (configIt != m_squadConfigs.end()) {
            applyConfigToSquadMember(member.get(), configIt->second);
        }
    }

    return member;
}

std::unique_ptr<SquadMember> SquadMemberFactory::createSquadMember(const std::string& typeName) const {
    auto type = stringToSquadMemberType(typeName);
    return createSquadMember(type);
}

std::unique_ptr<SquadMember> SquadMemberFactory::createSquadMemberWithLevel(GameTypes::SquadMemberType type, int level) const {
    auto member = createSquadMember(type);
    if (member) {
        applyLevelUpgrades(member.get(), level);
    }
    return member;
}

std::unique_ptr<SquadMember> SquadMemberFactory::createSquadMemberAtPosition(GameTypes::SquadMemberType type, int lane, int gridX) const {
    auto member = createSquadMember(type);
    if (member) {
        member->setGridPosition(gridX, lane);
    }
    return member;
}

std::vector<std::unique_ptr<SquadMember>> SquadMemberFactory::createSquadMembers(GameTypes::SquadMemberType type, int count) const {
    std::vector<std::unique_ptr<SquadMember>> members;
    members.reserve(count);

    for (int i = 0; i < count; ++i) {
        auto member = createSquadMember(type);
        if (member) {
            members.push_back(std::move(member));
        }
    }

    return members;
}

int SquadMemberFactory::getSquadMemberCost(GameTypes::SquadMemberType type) const {
    auto& config = ConfigLoader::getInstance();
    std::string unitName = squadMemberTypeToString(type);
    return config.getInt(unitName, "cost", 100);
}

int SquadMemberFactory::getSquadMemberUpgradeCost(GameTypes::SquadMemberType type, int currentLevel) const {
    int baseCost = getSquadMemberCost(type);
    return static_cast<int>(baseCost * std::pow(GameConstants::UPGRADE_COST_MULTIPLIER, currentLevel));
}

bool SquadMemberFactory::canAffordSquadMember(GameTypes::SquadMemberType type, int availableCoins) const {
    return availableCoins >= getSquadMemberCost(type);
}

ConfigLoader::SquadMemberConfig SquadMemberFactory::getSquadMemberConfig(GameTypes::SquadMemberType type) const {
    auto it = m_squadConfigs.find(type);
    if (it != m_squadConfigs.end()) {
        return it->second;
    }
    return ConfigLoader::SquadMemberConfig{};
}

ConfigLoader::SquadMemberConfig SquadMemberFactory::getSquadMemberConfig(const std::string& typeName) const {
    auto type = stringToSquadMemberType(typeName);
    return getSquadMemberConfig(type);
}

bool SquadMemberFactory::canCreateSquadMember(GameTypes::SquadMemberType type) const {
    return isConfigurationLoaded() && isSquadMemberUnlocked(type);
}

bool SquadMemberFactory::canCreateSquadMember(const std::string& typeName) const {
    auto type = stringToSquadMemberType(typeName);
    return canCreateSquadMember(type);
}

bool SquadMemberFactory::canPlaceSquadMember(GameTypes::SquadMemberType type, int lane, int gridX) const {
    return canCreateSquadMember(type) &&
        (lane >= 0 && lane < GameConstants::GRID_ROWS) &&
        (gridX >= 0 && gridX < GameConstants::GRID_COLUMNS);
}

void SquadMemberFactory::reloadConfigurations() {
    m_squadConfigs.clear();
    m_configurationsLoaded = false;
    loadConfigurations();
}

bool SquadMemberFactory::isConfigurationLoaded() const {
    return m_configurationsLoaded;
}

void SquadMemberFactory::setSquadMemberUnlocked(GameTypes::SquadMemberType type, bool unlocked) {
    m_unlockedTypes[type] = unlocked;
}

bool SquadMemberFactory::isSquadMemberUnlocked(GameTypes::SquadMemberType type) const {
    auto it = m_unlockedTypes.find(type);
    return (it != m_unlockedTypes.end()) ? it->second : false;
}

int SquadMemberFactory::getSquadMemberTypeCount() const {
    return static_cast<int>(m_nameToType.size());
}

std::vector<GameTypes::SquadMemberType> SquadMemberFactory::getAvailableTypes() const {
    std::vector<GameTypes::SquadMemberType> types;
    for (const auto& pair : m_nameToType) {
        types.push_back(pair.second);
    }
    return types;
}

std::vector<GameTypes::SquadMemberType> SquadMemberFactory::getUnlockedTypes() const {
    std::vector<GameTypes::SquadMemberType> types;
    for (const auto& pair : m_unlockedTypes) {
        if (pair.second) {
            types.push_back(pair.first);
        }
    }
    return types;
}

std::vector<std::string> SquadMemberFactory::getAvailableTypeNames() const {
    std::vector<std::string> names;
    for (const auto& pair : m_nameToType) {
        names.push_back(pair.first);
    }
    return names;
}

std::string SquadMemberFactory::squadMemberTypeToString(GameTypes::SquadMemberType type) const {
    switch (type) {
    case GameTypes::SquadMemberType::HeavyGunner: return "HeavyGunner";
    case GameTypes::SquadMemberType::Sniper: return "Sniper";
    case GameTypes::SquadMemberType::ShieldBearer: return "ShieldBearer";
    default: return "HeavyGunner";
    }
}

GameTypes::SquadMemberType SquadMemberFactory::stringToSquadMemberType(const std::string& name) const {
    auto it = m_nameToType.find(name);
    if (it != m_nameToType.end()) {
        return it->second;
    }
    return GameTypes::SquadMemberType::HeavyGunner;
}

void SquadMemberFactory::applyConfigToSquadMember(SquadMember* member, const ConfigLoader::SquadMemberConfig& config) const {
    if (!member) return;
    // Config is applied during member construction
}

void SquadMemberFactory::applyLevelUpgrades(SquadMember* member, int level) const {
    if (!member || level <= 1) return;

    // Apply upgrades for each level beyond 1
    for (int i = 2; i <= level && i <= GameConstants::MAX_UPGRADE_LEVEL; ++i) {
        // Upgrade logic would go here if implemented
    }
}