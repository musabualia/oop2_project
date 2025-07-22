// ================================
// Robot Manager - Entity Manager Pattern for Robot Lifecycle Management
// ================================
#include "Managers/RobotManager.h"
#include "Managers/SquadMemberManager.h"
#include "Entities/Robots/FireRobot.h"
#include "Entities/Robots/BasicRobot.h"
#include "Entities/Robots/StealthRobot.h"
#include "Core/GameManager.h"
#include "Physics/PhysicsWorld.h"
#include "Factories/RobotFactory.h"
#include "Core/Constants.h"
#include <algorithm>
#include <random>

// Constructor - Initialize factory and spawn points
RobotManager::RobotManager() {
    m_robotFactory = &RobotFactory::getInstance();
    initializeSpawnPoints();
}

// Main update loop - manages robot objectives and cleanup
void RobotManager::update(float dt) {
    EntityManager<Robot>::update(dt);     // Update all robots
    checkRobotObjectives();               // Check if robots reached base
    cleanupDeadRobots();                  // Remove inactive robots
}

// Render all active robots
void RobotManager::draw(sf::RenderWindow& window) const {
    EntityManager<Robot>::draw(window);
}

// Spawn robot of specific type in given lane (or random if -1)
void RobotManager::spawnRobot(GameTypes::RobotType type, int lane) {
    // Prevent spawning if factory unavailable or too many robots
    if (!m_robotFactory || getActiveRobotCount() >= 10) {
        return;
    }

    // Select lane: use provided lane or random
    int spawnLane = (lane >= 0 && lane < GameConstants::GRID_ROWS) ? lane : selectRandomLane();

    // Create robot using Factory Pattern
    auto robot = m_robotFactory->createRobot(type);
    if (!robot) {
        return;
    }

    // Position robot in correct lane
    robot->setLane(spawnLane);
    robot->setPosition(getSpawnPointForLane(spawnLane));

    // Connect robot to squad member detection system
    if (m_squadMemberManager) {
        robot->setSquadMemberManager(m_squadMemberManager);
    }

    // Add physics body for collision detection
    if (m_gameManager && m_gameManager->getPhysicsWorld()) {
        b2World* world = m_gameManager->getPhysicsWorld()->getWorld();
        if (world) {
            robot->createPhysicsBody(world);
        }
    }

    // Add to entity manager and track spawn count
    addEntity(std::move(robot));
    m_totalSpawned++;
}

// Connect to squad manager for FireRobot detection
void RobotManager::setSquadMemberManager(SquadMemberManager* squadManager) {
    m_squadMemberManager = squadManager;

    // Update all existing robots with squad manager reference
    for (const auto& robot : getEntities()) {
        if (robot) {
            robot->setSquadMemberManager(m_squadMemberManager);
        }
    }
}

// Get squad members in range for FireRobot targeting
std::vector<SquadMember*> RobotManager::getSquadMembersInRange(const sf::Vector2f& position, float radius) const {
    if (!m_squadMemberManager) {
        return {};
    }
    return m_squadMemberManager->getActiveSquadMembersInRange(position, radius);
}

// Get const reference to robot list
const std::vector<std::unique_ptr<Robot>>& RobotManager::getRobots() const {
    return getEntities();
}

// Find robots within bomb explosion radius
std::vector<Robot*> RobotManager::getRobotsInRange(const sf::Vector2f& center, float radius) const {
    return findEntities([center, radius](const Robot& robot) {
        sf::Vector2f diff = robot.getPosition() - center;
        float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        return distance <= radius;
        });
}

// Count active robots for wave management
int RobotManager::getActiveRobotCount() const {
    return static_cast<int>(activeSize());
}

// Handle robot reaching the base (game over condition)
void RobotManager::onRobotReachedBase(Robot* robot) {
    if (!robot) return;

    robot->setActive(false);

    // Notify game manager of base damage
    if (m_gameManager) {
        m_gameManager->onBaseReached();
    }
}

// Handle robot destruction (handled by EventSystem)
void RobotManager::onRobotDestroyed(Robot* robot) {
    if (!robot) return;
    // Robot destruction rewards are handled by the EventSystem
}

// Check if robots have reached objectives or died
void RobotManager::checkRobotObjectives() {
    for (const auto& robot : getEntities()) {
        if (!robot || !robot->isActive()) continue;

        // Check if robot reached the base (left side of screen)
        if (robot->getPosition().x < -200.0f) {
            onRobotReachedBase(robot.get());
        }

        // Mark dead robots for removal
        if (robot->isDead()) {
            onRobotDestroyed(robot.get());
        }
    }
}

// Remove inactive robots and maintain performance
void RobotManager::cleanupDeadRobots() {
    removeInactive();

    // Performance optimization: limit total robot count
    const int maxRobots = 8;
    if (getActiveRobotCount() > maxRobots) {
        // Remove robots that are far from action
        for (const auto& robot : getEntities()) {
            if (robot && robot->isActive() && robot->getPosition().x < 100.0f) {
                robot->setActive(false);
                break;
            }
        }
    }
}

// Calculate spawn position for specific lane
sf::Vector2f RobotManager::getSpawnPointForLane(int lane) const {
    // Clamp lane to valid range
    if (lane < 0 || lane >= GameConstants::GRID_ROWS) {
        lane = 0;
    }

    sf::Vector2f spawnPoint;
    spawnPoint.x = 1300.0f; // Spawn off-screen to the right
    // Calculate Y position based on lane and grid layout
    spawnPoint.y = GameConstants::GRID_OFFSET_Y + (lane * GameConstants::GRID_CELL_HEIGHT) + (GameConstants::GRID_CELL_HEIGHT / 2.0f);

    return spawnPoint;
}

// Random lane selection for varied gameplay
int RobotManager::selectRandomLane() const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, GameConstants::GRID_ROWS - 1);
    return dist(gen);
}

// Initialize spawn points for all lanes
void RobotManager::initializeSpawnPoints() {
    m_spawnPoints.clear();
    m_spawnPoints.reserve(GameConstants::GRID_ROWS);

    // Create spawn point for each lane
    for (int lane = 0; lane < GameConstants::GRID_ROWS; ++lane) {
        sf::Vector2f spawnPoint = getSpawnPointForLane(lane);
        m_spawnPoints.push_back(spawnPoint);
    }
}

// Legacy methods kept for interface compatibility
void RobotManager::setupFireRobotDetection() {
    // Individual robots handle their own detection now
}

void RobotManager::updateFireRobotDetection(float dt) {
    // Individual robots handle their own detection now
}