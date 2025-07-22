#include "Factories/CollectibleFactory.h"
#include "Entities/Collectibles/Coin.h"
#include "Entities/Collectibles/HealthPack.h"
#include "Systems/EventSystem.h"
#include "Utils/ConfigLoader.h"
#include <random>
#include <algorithm>

CollectibleFactory& CollectibleFactory::getInstance() {
    static CollectibleFactory instance;
    return instance;
}

void CollectibleFactory::initialize() {
    setupDefaultRobotDropChances();

    auto& config = ConfigLoader::getInstance();
    m_configLoaded = config.loadCollectiblesConfig();
}

std::unique_ptr<Collectible> CollectibleFactory::createCollectible(GameTypes::CollectibleType type) const {
    switch (type) {
    case GameTypes::CollectibleType::Coin:
        return createCoin(30); // Default robot reward if called without specific value
    case GameTypes::CollectibleType::HealthPack:
        return createHealthPack();
    default:
        return nullptr;
    }
}

std::unique_ptr<Collectible> CollectibleFactory::createCollectible(GameTypes::CollectibleType type, int value) const {
    switch (type) {
    case GameTypes::CollectibleType::Coin:
        return createCoin(value);
    case GameTypes::CollectibleType::HealthPack:
        return createHealthPack(value);
    default:
        return createCollectible(type);
    }
}

std::unique_ptr<Collectible> CollectibleFactory::createCoin(int value) const {
    return std::make_unique<Coin>(value);
}

std::unique_ptr<Collectible> CollectibleFactory::createHealthPack(int healAmount) const {
    return std::make_unique<HealthPack>(healAmount);
}

std::unique_ptr<Collectible> CollectibleFactory::spawnCollectibleAt(GameTypes::CollectibleType type, const sf::Vector2f& position) const {
    auto collectible = createCollectible(type);
    if (collectible) {
        collectible->spawn(position);
    }
    return collectible;
}

std::vector<std::unique_ptr<Collectible>> CollectibleFactory::createDropsFromRobot(GameTypes::RobotType robotType, const sf::Vector2f& position) const {
    std::vector<std::unique_ptr<Collectible>> drops;

    DropOutcome outcome = determineDropOutcome(robotType);

    switch (outcome) {
    case DropOutcome::Coin: {
        // Get robot's actual reward value from config
        auto& config = ConfigLoader::getInstance();
        int robotReward = config.getRobotReward(robotType);

        auto coin = createCoin(robotReward);
        if (coin) {
            coin->spawn(position);
            // Auto-collect the coin immediately and trigger visual effect
            static_cast<Coin*>(coin.get())->autoCollect();
            drops.push_back(std::move(coin));
        }

        // Pass robot's actual reward value
        EventSystem::getInstance().publish<CoinDropEvent>(position, robotReward);
        break;
    }

    case DropOutcome::HealthPack: {
        int healAmount = getHealthPackHealPercentage();
        auto pack = createHealthPack(healAmount);
        if (pack) {
            pack->spawn(position);
            drops.push_back(std::move(pack));
        }
        EventSystem::getInstance().publish<HealthPackDropEvent>(position, healAmount);
        break;
    }

    case DropOutcome::Nothing:
        break;
    }

    return drops;
}

CollectibleFactory::DropOutcome CollectibleFactory::determineDropOutcome(GameTypes::RobotType robotType) const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    float roll = dist(gen);
    float coinChance, healthPackChance, nothingChance;

    auto& config = ConfigLoader::getInstance();

    if (config.hasCollectiblesConfig()) {
        coinChance = config.getDropRate(robotType, "CoinChance");
        healthPackChance = config.getDropRate(robotType, "HealthPackChance");
        nothingChance = config.getDropRate(robotType, "NothingChance");

        if (coinChance < 0.0f || healthPackChance < 0.0f || nothingChance < 0.0f) {
            coinChance = -1.0f; // Fallback
        }
    }
    else {
        coinChance = -1.0f;
    }

    if (coinChance < 0.0f) {
        switch (robotType) {
        case GameTypes::RobotType::Basic:
            coinChance = 0.6f;
            healthPackChance = 0.4f;
            nothingChance = 0.0f;
            break;
        case GameTypes::RobotType::Fire:
            coinChance = 0.5f;
            healthPackChance = 0.3f;
            nothingChance = 0.2f;
            break;
        case GameTypes::RobotType::Stealth:
            coinChance = 0.5f;
            healthPackChance = 0.4f;
            nothingChance = 0.1f;
            break;
        default:
            coinChance = 0.5f;
            healthPackChance = 0.3f;
            nothingChance = 0.2f;
            break;
        }
    }

    if (roll < coinChance) {
        return DropOutcome::Coin;
    }
    else if (roll < coinChance + healthPackChance) {
        return DropOutcome::HealthPack;
    }
    else {
        return DropOutcome::Nothing;
    }
}

int CollectibleFactory::getHealthPackHealPercentage() const {
    auto& config = ConfigLoader::getInstance();
    if (config.hasCollectiblesConfig()) {
        return static_cast<int>(config.getHealthPackHealPercentage());
    }
    else {
        return 50;
    }
}

void CollectibleFactory::setRobotDropChance(GameTypes::RobotType robotType, float chance) {
    m_robotDropChances[robotType] = std::clamp(chance, 0.0f, 1.0f);
}

float CollectibleFactory::getRobotDropChance(GameTypes::RobotType robotType) const {
    auto it = m_robotDropChances.find(robotType);
    return it != m_robotDropChances.end() ? it->second : 0.3f;
}

void CollectibleFactory::setupDefaultRobotDropChances() {
    m_robotDropChances[GameTypes::RobotType::Basic] = 1.0f;
    m_robotDropChances[GameTypes::RobotType::Fire] = 0.8f;
    m_robotDropChances[GameTypes::RobotType::Stealth] = 0.9f;
}

sf::Vector2f CollectibleFactory::getRandomPositionNear(const sf::Vector2f& center, float spread) const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-spread, spread);
    return sf::Vector2f(center.x + dist(gen), center.y + dist(gen));
}