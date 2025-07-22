#pragma once
#include "Core/Constants.h"
#include "Core/Timer.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>

class RobotManager;

struct WaveData {
    int waveNumber;
    std::vector<GameTypes::RobotType> robotTypes;
    std::vector<int> robotCounts;
    float preparationTime = 10.0f;
    float spawnInterval = 2.0f;
    int bonusReward = 100;
    bool isBossWave = false;

    std::vector<GameTypes::RobotType> spawnOrder;
};

class WaveManager {
public:
    WaveManager();

    // Core functionality
    void update(float dt);

    // Wave control
    void startNextWave();
    void startWave(int waveNumber);
    void completeCurrentWave();
    void reset();

    // Wave queries
    int getCurrentWave() const;
    int getTotalWaves() const;
    GameTypes::WaveState getWaveState() const;
    float getTimeUntilNextWave() const;
    int getRemainingEnemies() const;
    bool isWaveActive() const;
    bool areAllWavesCompleted() const;

    // Level support
    void setCurrentLevel(int level);
    int getCurrentLevel() const;

    // Enemy spawning integration
    void setRobotManager(RobotManager* robotManager);

    // Wave countdown
    void showWaveCountdown(float seconds);
    void showWaveStarted(int waveNumber);
    bool isCountdownActive() const;
    float getCountdownTime() const;

    void renderCountdown(sf::RenderWindow& window);
    bool isShowingCountdown() const;

    // Callbacks for game events
    using WaveStartCallback = std::function<void(int)>;
    using WaveCompleteCallback = std::function<void(int, int)>;

    void setWaveStartCallback(WaveStartCallback callback);
    void setWaveCompleteCallback(WaveCompleteCallback callback);

private:
    // Wave data
    std::vector<WaveData> m_waves;
    int m_currentWaveIndex = 0;
    GameTypes::WaveState m_waveState = GameTypes::WaveState::Preparing;

    // Level tracking
    int m_currentLevel = 1;

    // Timing
    Timer m_preparationTimer;
    Timer m_spawnTimer;
    float m_preparationTime = 10.0f;

    RobotManager* m_robotManager = nullptr;
    int m_enemiesSpawned = 0;
    int m_currentSpawnIndex = 0;  // Index in spawnOrder instead of type-based tracking

    // Wave countdown
    bool m_countdownActive = false;
    float m_countdownTime = 0.0f;
    Timer m_countdownTimer;

    // ✅ NEW: Countdown display system
    GameTypes::CountdownPhase m_countdownPhase = GameTypes::CountdownPhase::None;
    Timer m_countdownDisplayTimer;
    sf::Text m_countdownText;
    sf::Text m_waveText;
    int m_countdownNumber = 5;
    bool m_countdownTextSetup = false;
    bool m_isFirstWave = true;

    // Callbacks
    WaveStartCallback m_waveStartCallback;
    WaveCompleteCallback m_waveCompleteCallback;

    // Helper methods
    void loadDefaultWaves();
    void updateWaveSpawning(float dt);
    void updateCountdown(float dt);
    void spawnNextEnemy();
    void checkWaveCompletion();
    WaveData createDynamicWave(int level, int waveNumber) const;
    int getCurrentWaveEnemyCount() const;

    // Countdown display methods
    void setupCountdownText();
    void updateCountdownDisplay(float dt);
    void nextCountdownPhase();
    void updateTextForCurrentPhase();
    float calculateCountdownAlpha() const;
};