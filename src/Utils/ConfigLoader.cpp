// ================================
// src/Utils/ConfigLoader.cpp
// ================================
#include "Utils/ConfigLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

// Define required config files
const std::vector<std::string> ConfigLoader::REQUIRED_CONFIG_FILES = {
    "game.cfg",
    "robots.cfg",
    "units.cfg",
    "physics.cfg",
    "projectiles.cfg",
    "collectibles.cfg"
};

ConfigLoader& ConfigLoader::getInstance() {
    static ConfigLoader instance;
    return instance;
}

// Initialize all configurations at startup
bool ConfigLoader::initializeAllConfigs() {
    clearError();
    m_loadedConfigs.clear();
    m_config.clear();

    return m_initialized;
}

// Validation methods
bool ConfigLoader::validateAllConfigs() const {
    return getMissingConfigs().empty();
}

std::vector<std::string> ConfigLoader::getMissingConfigs() const {
    std::vector<std::string> missing;

    for (const auto& configFile : REQUIRED_CONFIG_FILES) {
        auto it = std::find(m_loadedConfigs.begin(), m_loadedConfigs.end(), configFile);
        if (it == m_loadedConfigs.end()) {
            missing.push_back(configFile);
        }
    }

    return missing;
}

int ConfigLoader::getProjectileDamage(GameTypes::ProjectileType type) const {
    std::string projectileSection;

    switch (type) {
    case GameTypes::ProjectileType::Bullet:
        projectileSection = "SquadBullet";
        break;
    case GameTypes::ProjectileType::SniperBullet:
        projectileSection = "SniperBullet";
        break;
    case GameTypes::ProjectileType::RobotBullet:
        projectileSection = "RobotBullet";
        break;
    default:
        projectileSection = "SquadBullet";
        break;
    }

    int damage = getInt(projectileSection, "damage", 25);

    return damage;
}


float ConfigLoader::getProjectileSpeed(GameTypes::ProjectileType type) const {
    std::string projectileSection;

    switch (type) {
    case GameTypes::ProjectileType::Bullet:
        projectileSection = "SquadBullet";
        break;
    case GameTypes::ProjectileType::SniperBullet:
        projectileSection = "SniperBullet";
        break;
    case GameTypes::ProjectileType::RobotBullet:
        projectileSection = "RobotBullet";
        break;
    default:
        projectileSection = "SquadBullet";
        break;
    }

    return getFloat(projectileSection, "speed", 400.0f);
}

// Physics configuration getters
float ConfigLoader::getPixelsPerMeter() const {
    return getFloat("Physics", "pixelsPerMeter", 30.0f);
}

float ConfigLoader::getGravityX() const {
    return getFloat("Physics", "gravityX", 0.0f);
}

float ConfigLoader::getGravityY() const {
    return getFloat("Physics", "gravityY", 9.8f);
}

int ConfigLoader::getVelocityIterations() const {
    return getInt("Physics", "velocityIterations", 6);
}

int ConfigLoader::getPositionIterations() const {
    return getInt("Physics", "positionIterations", 2);
}

// Load projectile config with all fields
ConfigLoader::ProjectileConfig ConfigLoader::loadProjectileConfig(const std::string& projectileName) const {
    ProjectileConfig config;

    // Set projectile type
    if (projectileName == "SquadBullet") config.type = GameTypes::ProjectileType::Bullet;
    else if (projectileName == "SniperBullet") config.type = GameTypes::ProjectileType::SniperBullet;
    else if (projectileName == "RobotBullet") config.type = GameTypes::ProjectileType::RobotBullet;
    else config.type = GameTypes::ProjectileType::Bullet;

    // Load all projectile properties
    config.speed = getFloat(projectileName, "speed", 400.0f);
    config.texture = getString(projectileName, "texture", "bullet.png");
    config.scale = getFloat(projectileName, "scale", 0.03f);
    config.maxRange = getFloat(projectileName, "maxRange", 300.0f);
    config.damage = getInt(projectileName, "damage", 25);

    // Parse spawn offset (format: "x,y")
    std::string offsetStr = getString(projectileName, "spawnOffset", "50,-14");
    size_t commaPos = offsetStr.find(',');
    if (commaPos != std::string::npos) {
        config.spawnOffset.x = std::stof(offsetStr.substr(0, commaPos));
        config.spawnOffset.y = std::stof(offsetStr.substr(commaPos + 1));
    }
    else {
        config.spawnOffset = sf::Vector2f(50.0f, -14.0f);
    }

    return config;
}

// Load physics configuration
ConfigLoader::PhysicsConfig ConfigLoader::loadPhysicsConfig() const {
    PhysicsConfig config;

    // Main physics settings
    config.pixelsPerMeter = getFloat("Physics", "pixelsPerMeter", 30.0f);
    config.gravityX = getFloat("Physics", "gravityX", 0.0f);
    config.gravityY = getFloat("Physics", "gravityY", 9.8f);
    config.velocityIterations = getInt("Physics", "velocityIterations", 6);
    config.positionIterations = getInt("Physics", "positionIterations", 2);

    // Robot physics
    config.robotRadius = getFloat("RobotPhysics", "defaultRadius", 15.0f);
    config.robotMass = getFloat("RobotPhysics", "defaultMass", 1.0f);
    config.robotFriction = getFloat("RobotPhysics", "defaultFriction", 0.3f);

    // Squad member physics
    config.squadWidth = getFloat("SquadMemberPhysics", "defaultWidth", 40.0f);
    config.squadHeight = getFloat("SquadMemberPhysics", "defaultHeight", 40.0f);
    config.squadMass = getFloat("SquadMemberPhysics", "defaultMass", 0.0f);

    // Projectile physics
    config.bulletRadius = getFloat("ProjectilePhysics", "bulletRadius", 2.0f);
    config.rocketRadius = getFloat("ProjectilePhysics", "rocketRadius", 5.0f);
    config.bulletMass = getFloat("ProjectilePhysics", "bulletMass", 0.1f);

    return config;
}

bool ConfigLoader::loadFromFile(const std::string& filename) {
    clearError();

    std::string content = readFileContent(filename);
    if (content.empty()) {
        setError("Failed to read file: " + filename);
        return false;
    }

    return parseINI(content);
}

void ConfigLoader::clearConfig() {
    m_config.clear();
    m_loadedConfigs.clear();
    m_initialized = false;
    clearError();
}

bool ConfigLoader::getBool(const std::string& section, const std::string& key, bool defaultValue) const {
    if (!hasValue(section, key)) {
        return defaultValue;
    }
    return stringToBool(m_config.at(section).at(key));
}

int ConfigLoader::getInt(const std::string& section, const std::string& key, int defaultValue) const {
    if (!hasValue(section, key)) {
        return defaultValue;
    }

    try {
        std::string rawValue = m_config.at(section).at(key);
        int result = std::stoi(rawValue);
        return result;
    }
    catch (const std::exception& e) {
        return defaultValue;
    }
}

float ConfigLoader::getFloat(const std::string& section, const std::string& key, float defaultValue) const {
    if (!hasValue(section, key)) {
        return defaultValue;
    }
    try {
        return std::stof(m_config.at(section).at(key));
    }
    catch (const std::exception&) {
        return defaultValue;
    }
}

std::string ConfigLoader::getString(const std::string& section, const std::string& key, const std::string& defaultValue) const {
    if (!hasValue(section, key)) {
        return defaultValue;
    }
    return m_config.at(section).at(key);
}

bool ConfigLoader::hasSection(const std::string& section) const {
    return m_config.find(section) != m_config.end();
}

bool ConfigLoader::hasValue(const std::string& section, const std::string& key) const {
    auto sectionIt = m_config.find(section);
    if (sectionIt == m_config.end()) {
        return false;
    }
    return sectionIt->second.find(key) != sectionIt->second.end();
}

int ConfigLoader::getUnitCost(GameTypes::SquadMemberType type) const {
    std::string unitName;
    switch (type) {
    case GameTypes::SquadMemberType::HeavyGunner:
        unitName = "HeavyGunner";
        break;
    case GameTypes::SquadMemberType::Sniper:
        unitName = "Sniper";
        break;
    case GameTypes::SquadMemberType::ShieldBearer:
        unitName = "ShieldBearer";
        break;
    default:
        unitName = "HeavyGunner";
        break;
    }
    return getInt(unitName, "cost", 100);
}

int ConfigLoader::getRobotReward(GameTypes::RobotType type) const {
    std::string robotSection;
    switch (type) {
    case GameTypes::RobotType::Basic:
        robotSection = "BasicRobot";
        break;
    case GameTypes::RobotType::Fire:
        robotSection = "FireRobot";
        break;
    case GameTypes::RobotType::Stealth:
        robotSection = "StealthRobot";
        break;
    default:
        robotSection = "BasicRobot";
        break;
    }
    return getInt(robotSection, "reward", 30);
}

float ConfigLoader::getDropRate(GameTypes::RobotType robotType, const std::string& dropType) const {
    std::string robotPrefix;
    switch (robotType) {
    case GameTypes::RobotType::Basic:
        robotPrefix = "BasicRobot";
        break;
    case GameTypes::RobotType::Fire:
        robotPrefix = "FireRobot";
        break;
    case GameTypes::RobotType::Stealth:
        robotPrefix = "StealthRobot";
        break;
    default:
        return -1.0f;
    }

    std::string key = robotPrefix + dropType;
    return getFloat("DropRates", key, -1.0f);
}

bool ConfigLoader::hasCollectiblesConfig() const {
    return hasSection("DropRates") && hasSection("HealthPack");
}

bool ConfigLoader::loadCollectiblesConfig() {
    return loadFromFile("collectibles.cfg");
}

float ConfigLoader::getHealthPackHealPercentage() const {
    return getFloat("HealthPack", "healPercentage", 50.0f);
}

ConfigLoader::RobotConfig ConfigLoader::loadRobotConfig(const std::string& robotName) const {
    RobotConfig config;

    if (robotName == "BasicRobot") config.type = GameTypes::RobotType::Basic;
    else if (robotName == "FireRobot") config.type = GameTypes::RobotType::Fire;
    else if (robotName == "StealthRobot") config.type = GameTypes::RobotType::Stealth;
    else config.type = GameTypes::RobotType::Basic;

    config.health = getInt(robotName, "health", 100);
    config.speed = getFloat(robotName, "speed", 50.0f);
    config.damage = getInt(robotName, "damage", 25);
    config.reward = getInt(robotName, "reward", 25);
    config.attackRange = getFloat(robotName, "attackRange", 100.0f);
    config.meleeAttackDamage = getInt(robotName, "meleeAttackDamage", config.damage);
    config.bulletCooldown = getFloat(robotName, "bulletCooldown", 0.0f);

    return config;
}

ConfigLoader::SquadMemberConfig ConfigLoader::loadSquadMemberConfig(const std::string& unitName) const {
    SquadMemberConfig config;

    if (unitName == "HeavyGunner") config.type = GameTypes::SquadMemberType::HeavyGunner;
    else if (unitName == "Sniper") config.type = GameTypes::SquadMemberType::Sniper;
    else if (unitName == "ShieldBearer") config.type = GameTypes::SquadMemberType::ShieldBearer;
    else config.type = GameTypes::SquadMemberType::HeavyGunner;

    config.cost = getInt(unitName, "cost", 100);
    config.range = getFloat(unitName, "range", 200.0f);
    config.health = getInt(unitName, "health", 150);

    return config;
}

ConfigLoader::GameConfig ConfigLoader::loadGameConfig() const {
    GameConfig config;

    config.windowWidth = getInt("Window", "width", 1200);
    config.windowHeight = getInt("Window", "height", 800);
    config.windowTitle = getString("Window", "title", "Special Forces vs Robots");

    config.masterVolume = getFloat("Audio", "masterVolume", 100.0f);
    config.musicVolume = getFloat("Audio", "musicVolume", 70.0f);
    config.sfxVolume = getFloat("Audio", "sfxVolume", 80.0f);

    config.initialCoins = getInt("Gameplay", "initialCoins", 200);
    config.baseHealth = getInt("Gameplay", "baseHealth", 100);
    config.numLanes = getInt("Gameplay", "numLanes", 3);

    return config;
}

std::string ConfigLoader::getLastError() const {
    return m_lastError;
}

bool ConfigLoader::hasErrors() const {
    return !m_lastError.empty();
}

bool ConfigLoader::parseINI(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    std::string currentSection;

    while (std::getline(stream, line)) {
        line = trim(line);

        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }

        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.length() - 2);
            continue;
        }

        size_t equalPos = line.find('=');
        if (equalPos != std::string::npos && !currentSection.empty()) {
            std::string key = trim(line.substr(0, equalPos));
            std::string value = trim(line.substr(equalPos + 1));
            m_config[currentSection][key] = value;
        }
    }

    return true;
}

std::string ConfigLoader::readFileContent(const std::string& filename) const {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }

    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

std::string ConfigLoader::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";

    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool ConfigLoader::stringToBool(const std::string& str) const {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
}

void ConfigLoader::setError(const std::string& error) const {
    m_lastError = error;
}

void ConfigLoader::clearError() const {
    m_lastError.clear();
}

void ConfigLoader::addLoadedConfig(const std::string& configName) {
    m_loadedConfigs.push_back(configName);
}