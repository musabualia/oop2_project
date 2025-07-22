#pragma once
#include "Managers/EntityManager.h"
#include "Entities/Base/Robot.h"
#include "Core/Constants.h"
#include "Core/Timer.h"
#include <SFML/Graphics.hpp>
#include <memory>

class WaveManager;
class RobotFactory;
class GameManager;
class SquadMemberManager;

class RobotManager : public EntityManager<Robot> {
public:
    RobotManager();                    // Constructor
    virtual ~RobotManager() = default; // Destructor

    void update(float dt) override;               // Update robots
    void draw(sf::RenderWindow& window) const override; // Render robots

    void spawnRobot(GameTypes::RobotType type, int lane = -1); // Spawn robot of specific type
    const std::vector<std::unique_ptr<Robot>>& getRobots() const; // Get list of robots

    std::vector<Robot*> getRobotsInRange(const sf::Vector2f& center, float radius) const; // Robots in bomb radius

    int getActiveRobotCount() const;

    void onRobotReachedBase(Robot* robot); // When robot reaches base
    void onRobotDestroyed(Robot* robot);   // When robot is destroyed

    void setGameManager(GameManager* gameManager) { m_gameManager = gameManager; } // Link to game manager

    void setSquadMemberManager(SquadMemberManager* squadManager); // Set squad manager ref
    std::vector<class SquadMember*> getSquadMembersInRange(const sf::Vector2f& position, float radius) const; // Get nearby squad members

    // FireRobot detection management
    void setupFireRobotDetection();
    void updateFireRobotDetection(float dt);

private:
    GameManager* m_gameManager = nullptr;            // Link to game manager
    RobotFactory* m_robotFactory = nullptr;          // Robot factory instance

    SquadMemberManager* m_squadMemberManager = nullptr; // Link to squad manager for FireRobot

    std::vector<sf::Vector2f> m_spawnPoints;         // Predefined spawn points
    int m_totalSpawned = 0;                          // Total robots spawned

    void checkRobotObjectives();                     // Check robot objectives
    void cleanupDeadRobots();                        // Remove dead robots
    sf::Vector2f getSpawnPointForLane(int lane) const; // Get spawn point by lane
    int selectRandomLane() const;                    // Random lane selector
    void initializeSpawnPoints();                    // Setup initial spawn points
};
