#include "Factories/RobotFactory.h"
#include "Entities/Robots/BasicRobot.h"
#include "Entities/Robots/FireRobot.h"
#include "Entities/Robots/StealthRobot.h"
#include "Utils/ConfigLoader.h"
#include <random>

RobotFactory& RobotFactory::getInstance() {
    static RobotFactory instance;
    return instance;
}

void RobotFactory::initialize() {
    setupNameMappings();
    loadConfigurations();
}

void RobotFactory::setupNameMappings() {
    m_nameToType["BasicRobot"] = GameTypes::RobotType::Basic;
    m_nameToType["FireRobot"] = GameTypes::RobotType::Fire;
    m_nameToType["StealthRobot"] = GameTypes::RobotType::Stealth;
}

void RobotFactory::loadConfigurations() {
    loadRobotConfiguration(GameTypes::RobotType::Basic, "BasicRobot");
    loadRobotConfiguration(GameTypes::RobotType::Fire, "FireRobot");
    loadRobotConfiguration(GameTypes::RobotType::Stealth, "StealthRobot");

    m_configurationsLoaded = true;
}

void RobotFactory::loadRobotConfiguration(GameTypes::RobotType type, const std::string& configName) {
    try {
        auto config = ConfigLoader::getInstance().loadRobotConfig(configName);
        m_robotConfigs[type] = config;
    }
    catch (const std::exception&) {
        // Use default configuration on error
    }
}

std::unique_ptr<Robot> RobotFactory::createRobot(GameTypes::RobotType type) const {
    if (!canCreateRobot(type)) {
        return nullptr;
    }

    std::unique_ptr<Robot> robot;

    switch (type) {
    case GameTypes::RobotType::Basic:
        robot = std::make_unique<BasicRobot>();
        break;
    case GameTypes::RobotType::Fire:
        robot = std::make_unique<FireRobot>();
        break;
    case GameTypes::RobotType::Stealth:
        robot = std::make_unique<StealthRobot>();
        break;
    default:
        return nullptr;
    }

    // Configuration is applied during robot construction
    return robot;
}

std::unique_ptr<Robot> RobotFactory::createRobot(const std::string& typeName) const {
    auto type = stringToRobotType(typeName);
    return createRobot(type);
}

std::vector<std::unique_ptr<Robot>> RobotFactory::createRobots(GameTypes::RobotType type, int count) const {
    std::vector<std::unique_ptr<Robot>> robots;
    robots.reserve(count);

    for (int i = 0; i < count; ++i) {
        auto robot = createRobot(type);
        if (robot) {
            robots.push_back(std::move(robot));
        }
    }

    return robots;
}

std::vector<std::unique_ptr<Robot>> RobotFactory::createMixedRobots(const std::vector<GameTypes::RobotType>& types) const {
    std::vector<std::unique_ptr<Robot>> robots;
    robots.reserve(types.size());

    for (auto type : types) {
        auto robot = createRobot(type);
        if (robot) {
            robots.push_back(std::move(robot));
        }
    }

    return robots;
}

std::unique_ptr<Robot> RobotFactory::createRandomRobot() const {
    auto availableTypes = getAvailableTypes();
    if (availableTypes.empty()) {
        return nullptr;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, static_cast<int>(availableTypes.size()) - 1);

    GameTypes::RobotType selectedType = availableTypes[dist(gen)];
    return createRobot(selectedType);
}

std::unique_ptr<Robot> RobotFactory::createRandomRobotForWave(int waveNumber) const {
    std::vector<GameTypes::RobotType> availableTypes;
    availableTypes.push_back(GameTypes::RobotType::Basic);

    if (waveNumber >= 2) {
        availableTypes.push_back(GameTypes::RobotType::Fire);
    }
    if (waveNumber >= 3) {
        availableTypes.push_back(GameTypes::RobotType::Stealth);
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<int> weights;

    for (auto type : availableTypes) {
        int weight = 1;

        switch (type) {
        case GameTypes::RobotType::Basic:
            weight = std::max(1, 10 - waveNumber);
            break;
        case GameTypes::RobotType::Fire:
            weight = 3;
            break;
        case GameTypes::RobotType::Stealth:
            weight = 2;
            break;
        }

        weights.push_back(weight);
    }

    std::discrete_distribution<> dist(weights.begin(), weights.end());
    GameTypes::RobotType selectedType = availableTypes[dist(gen)];

    return createRobot(selectedType);
}

ConfigLoader::RobotConfig RobotFactory::getRobotConfig(GameTypes::RobotType type) const {
    auto it = m_robotConfigs.find(type);
    if (it != m_robotConfigs.end()) {
        return it->second;
    }
    return ConfigLoader::RobotConfig{};
}

ConfigLoader::RobotConfig RobotFactory::getRobotConfig(const std::string& typeName) const {
    auto type = stringToRobotType(typeName);
    return getRobotConfig(type);
}

bool RobotFactory::canCreateRobot(GameTypes::RobotType type) const {
    return isConfigurationLoaded() && m_robotConfigs.find(type) != m_robotConfigs.end();
}

bool RobotFactory::canCreateRobot(const std::string& typeName) const {
    auto type = stringToRobotType(typeName);
    return canCreateRobot(type);
}

void RobotFactory::reloadConfigurations() {
    m_robotConfigs.clear();
    m_configurationsLoaded = false;
    loadConfigurations();
}

bool RobotFactory::isConfigurationLoaded() const {
    return m_configurationsLoaded;
}

int RobotFactory::getRobotTypeCount() const {
    return static_cast<int>(m_nameToType.size());
}

std::vector<GameTypes::RobotType> RobotFactory::getAvailableTypes() const {
    std::vector<GameTypes::RobotType> types;
    for (const auto& pair : m_nameToType) {
        types.push_back(pair.second);
    }
    return types;
}

std::vector<std::string> RobotFactory::getAvailableTypeNames() const {
    std::vector<std::string> names;
    for (const auto& pair : m_nameToType) {
        names.push_back(pair.first);
    }
    return names;
}

std::string RobotFactory::robotTypeToString(GameTypes::RobotType type) const {
    switch (type) {
    case GameTypes::RobotType::Basic: return "BasicRobot";
    case GameTypes::RobotType::Fire: return "FireRobot";
    case GameTypes::RobotType::Stealth: return "StealthRobot";
    default: return "BasicRobot";
    }
}

GameTypes::RobotType RobotFactory::stringToRobotType(const std::string& name) const {
    auto it = m_nameToType.find(name);
    if (it != m_nameToType.end()) {
        return it->second;
    }
    return GameTypes::RobotType::Basic;
}

void RobotFactory::applyConfigToRobot(Robot* robot, const ConfigLoader::RobotConfig& config) const {
    // Configuration is applied during robot construction
    // This method is kept for interface compatibility
}