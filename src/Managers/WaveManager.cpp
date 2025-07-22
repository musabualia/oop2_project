// ================================
// Wave Manager - State Pattern for Progressive Wave Management and Enemy Spawning
// ================================
#include "Managers/WaveManager.h"
#include "Managers/RobotManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/DynamicWaveGenerator.h"
#include <algorithm>

// Constructor - Initialize wave system and countdown
WaveManager::WaveManager() {
    loadDefaultWaves();                    // Generate waves for current level
    m_preparationTimer.setDuration(m_preparationTime);
    setupCountdownText();                  // Initialize countdown display
}
// Main update loop - handles all wave states
void WaveManager::update(float dt) {
    updateCountdown(dt);           // Update wave countdown timer
    updateCountdownDisplay(dt);    // Update visual countdown display
    updateWaveSpawning(dt);        // Handle enemy spawning during active waves
}
// Update countdown timer and transition logic
void WaveManager::updateCountdown(float dt) {
    if (m_countdownActive) {
        m_countdownTimer.update(dt);

        // Start wave when countdown display shows "Complete"
        if (m_countdownPhase == GameTypes::CountdownPhase::Complete) {
            m_countdownActive = false;
            startNextWave();
        }
    }
}
// Update visual countdown display phases
void WaveManager::updateCountdownDisplay(float dt) {
    if (m_countdownPhase == GameTypes::CountdownPhase::None) return;

    m_countdownDisplayTimer.update(dt);
    if (m_countdownDisplayTimer.isElapsed()) {
        nextCountdownPhase();   // Move to next countdown phase
    }
}
// Handle enemy spawning during active waves
void WaveManager::updateWaveSpawning(float dt) {
    if (m_waveState != GameTypes::WaveState::Active || !m_robotManager) return;

    m_spawnTimer.update(dt);

    // Spawn next enemy when timer elapsed and enemies remaining
    if (m_spawnTimer.isElapsed() && m_currentSpawnIndex < getCurrentWaveEnemyCount()) {
        spawnNextEnemy();

        // Random spawn interval for unpredictable gameplay
        float minInterval = 4.0f;
        float maxInterval = 5.0f;
        float randomInterval = minInterval + (static_cast<float>(rand()) / RAND_MAX) * (maxInterval - minInterval);

        m_spawnTimer.setDuration(randomInterval);
        m_spawnTimer.restart();
    }

    checkWaveCompletion();   // Check if wave is complete
}

// Start the next wave in sequence
void WaveManager::startNextWave() {
    if (m_currentWaveIndex >= m_waves.size()) {
        m_waveState = GameTypes::WaveState::AllCompleted;
        return;
    }

    // Transition to active state
    m_waveState = GameTypes::WaveState::Active;
    m_enemiesSpawned = 0;
    m_currentSpawnIndex = 0;

    showWaveStarted(m_currentWaveIndex + 1);   // Mark first wave done

    // Start spawning immediately after countdown
    m_spawnTimer.setDuration(0.5f);  // First robot spawns quickly
    m_spawnTimer.restart();

    // Notify game of wave start
    if (m_waveStartCallback) {
        m_waveStartCallback(m_currentWaveIndex + 1);
    }
}
// Start specific wave by number
void WaveManager::startWave(int waveNumber) {
    m_currentWaveIndex = std::clamp(waveNumber - 1, 0, static_cast<int>(m_waves.size()) - 1);
    startNextWave();
}

// Complete current wave and check for victory
void WaveManager::completeCurrentWave() {
    if (m_waveState == GameTypes::WaveState::Active) {
        m_waveState = GameTypes::WaveState::Completed;

        // Check for victory BEFORE incrementing wave
        if (m_currentWaveIndex + 1 >= m_waves.size()) {
            // All waves completed - transition to victory
            m_waveState = GameTypes::WaveState::AllCompleted;

            if (m_waveCompleteCallback) {
                int bonusReward = getCurrentWave() < m_waves.size() ?
                    m_waves[getCurrentWave() - 1].bonusReward : 100;
                m_waveCompleteCallback(getCurrentWave(), bonusReward);
            }
            return;
        }

        // Move to next wave
        m_currentWaveIndex++;

        // Notify completion with bonus
        if (m_waveCompleteCallback) {
            int bonusReward = getCurrentWave() < m_waves.size() ?
                m_waves[getCurrentWave() - 1].bonusReward : 100;
            m_waveCompleteCallback(getCurrentWave(), bonusReward);
        }

        // Start countdown for next wave
        showWaveCountdown(5.0f);
    }
}
// Reset wave manager to initial state
void WaveManager::reset() {
    m_currentWaveIndex = 0;
    m_waveState = GameTypes::WaveState::Preparing;
    m_enemiesSpawned = 0;
    m_currentSpawnIndex = 0;
    m_countdownActive = false;
    m_countdownPhase = GameTypes::CountdownPhase::None;
    m_isFirstWave = true;
}

// Wave Query Methods

int WaveManager::getCurrentWave() const {
    return m_currentWaveIndex + 1;
}

int WaveManager::getTotalWaves() const {
    return static_cast<int>(m_waves.size());
}

GameTypes::WaveState WaveManager::getWaveState() const {
    return m_waveState;
}

float WaveManager::getTimeUntilNextWave() const {
    return m_countdownActive ? m_countdownTimer.getRemaining() : 0.0f;
}

int WaveManager::getRemainingEnemies() const {
    if (m_waveState != GameTypes::WaveState::Active || m_currentWaveIndex >= m_waves.size()) {
        return 0;
    }
    return getCurrentWaveEnemyCount() - m_enemiesSpawned;
}

bool WaveManager::isWaveActive() const {
    return m_waveState == GameTypes::WaveState::Active;
}

bool WaveManager::areAllWavesCompleted() const {
    return m_waveState == GameTypes::WaveState::AllCompleted;
}

// Level support
void WaveManager::setCurrentLevel(int level) {
    m_currentLevel = level;
    loadDefaultWaves();   // Regenerate waves for new level
}

int WaveManager::getCurrentLevel() const {
    return m_currentLevel;
}

// Robot spawning integration
void WaveManager::setRobotManager(RobotManager* robotManager) {
    m_robotManager = robotManager;
}

// Wave countdown system
void WaveManager::showWaveCountdown(float seconds) {
    m_countdownTime = seconds;
    m_countdownTimer.setDuration(seconds);
    m_countdownTimer.restart();
    m_countdownActive = true;

    // Different countdown for first wave vs subsequent waves
    if (m_isFirstWave) {
        // Show "GET READY FOR COMBAT!" for first wave
        m_countdownPhase = GameTypes::CountdownPhase::GetReady;
        m_countdownDisplayTimer.setDuration(GameConstants::Countdown::GET_READY_DURATION);
        m_countdownDisplayTimer.restart();
        updateTextForCurrentPhase();
    }
    else {
        // Skip "GET READY" for subsequent waves, go straight to countdown
        m_countdownPhase = GameTypes::CountdownPhase::WaveDisplay;
        m_countdownNumber = 5;
        m_countdownDisplayTimer.setDuration(GameConstants::Countdown::COUNTDOWN_DURATION);
        m_countdownDisplayTimer.restart();
        updateTextForCurrentPhase();
    }
}

void WaveManager::showWaveStarted(int waveNumber) {
    m_isFirstWave = false;  // Mark that first wave is done
}

bool WaveManager::isCountdownActive() const {
    return m_countdownActive;
}

float WaveManager::getCountdownTime() const {
    return m_countdownTime;
}

// Countdown display rendering
void WaveManager::renderCountdown(sf::RenderWindow& window) {
    if (m_countdownPhase == GameTypes::CountdownPhase::None || !m_countdownTextSetup) {
        return;
    }

    sf::Vector2u windowSize = window.getSize();

    // Render based on current countdown phase
    if (m_countdownPhase == GameTypes::CountdownPhase::GetReady) {
        // Show "GET READY FOR COMBAT!" with fade effect
        float alpha = calculateCountdownAlpha();
        sf::Color currentColor = sf::Color::White;
        currentColor.a = static_cast<sf::Uint8>(std::clamp(alpha, 0.0f, 255.0f));
        m_countdownText.setFillColor(currentColor);

        // Center the text
        sf::FloatRect textBounds = m_countdownText.getLocalBounds();
        m_countdownText.setPosition(
            (windowSize.x - textBounds.width) / 2.0f,
            windowSize.y * 0.3f
        );

        window.draw(m_countdownText);
    }
    else if (m_countdownPhase == GameTypes::CountdownPhase::WaveDisplay) {
        // Draw wave text (solid white, no fading)
        m_waveText.setFillColor(sf::Color::White);
        sf::FloatRect waveTextBounds = m_waveText.getLocalBounds();
        m_waveText.setPosition(
            (windowSize.x - waveTextBounds.width) / 2.0f,
            windowSize.y * 0.35f
        );
        window.draw(m_waveText);

        // Draw countdown number (gold color with fade effect)
        float alpha = calculateCountdownAlpha();
        sf::Color countdownColor = sf::Color(255, 215, 0);  // Gold
        countdownColor.a = static_cast<sf::Uint8>(std::clamp(alpha, 0.0f, 255.0f));
        m_countdownText.setFillColor(countdownColor);

        sf::FloatRect countdownBounds = m_countdownText.getLocalBounds();
        m_countdownText.setPosition(
            (windowSize.x - countdownBounds.width) / 2.0f,
            windowSize.y * 0.45f
        );
        window.draw(m_countdownText);
    }
}

bool WaveManager::isShowingCountdown() const {
    return m_countdownActive || m_countdownPhase != GameTypes::CountdownPhase::None;
}

// Callback system for game events
void WaveManager::setWaveStartCallback(WaveStartCallback callback) {
    m_waveStartCallback = callback;
}

void WaveManager::setWaveCompleteCallback(WaveCompleteCallback callback) {
    m_waveCompleteCallback = callback;
}

// Generate waves using Strategy Pattern (DynamicWaveGenerator)
void WaveManager::loadDefaultWaves() {
    m_waves.clear();
    for (int wave = 1; wave <= 3; ++wave) {
        m_waves.push_back(createDynamicWave(m_currentLevel, wave));
    }
}

// Spawn next enemy from current wave's spawn order
void WaveManager::spawnNextEnemy() {
    if (!m_robotManager || m_currentWaveIndex >= m_waves.size()) return;

    const WaveData& currentWave = m_waves[m_currentWaveIndex];

    // Use randomized spawn order for unpredictable gameplay
    if (!currentWave.spawnOrder.empty() && m_currentSpawnIndex < currentWave.spawnOrder.size()) {
        GameTypes::RobotType typeToSpawn = currentWave.spawnOrder[m_currentSpawnIndex];
        int randomLane = rand() % GameConstants::GRID_ROWS;

        m_robotManager->spawnRobot(typeToSpawn, randomLane);
        m_currentSpawnIndex++;
        m_enemiesSpawned++;
    }
}
// Check if wave is complete (all spawned and all defeated)
void WaveManager::checkWaveCompletion() {
    bool allSpawned = (m_enemiesSpawned >= getCurrentWaveEnemyCount());
    bool allDefeated = (m_robotManager->getActiveRobotCount() == 0);

    if (m_waveState == GameTypes::WaveState::Active && allSpawned && allDefeated) {
        completeCurrentWave();
    }
}
// Create wave using DynamicWaveGenerator (Strategy Pattern)
WaveData WaveManager::createDynamicWave(int level, int waveNumber) const {
    WaveData wave;
    wave.waveNumber = waveNumber;
    wave.preparationTime = 5.0f;
    wave.spawnInterval = std::max(2.0f, 4.5f - (level * 0.2f));  // Faster spawning at higher levels
    wave.bonusReward = 50 + (level * 25) + (waveNumber * 10);
    wave.isBossWave = (waveNumber == 3);

    // Generate wave composition using Strategy Pattern
    auto composition = DynamicWaveGenerator::generateWave(level, waveNumber);

    // Convert composition to wave data
    if (composition.basicRobots > 0) {
        wave.robotTypes.push_back(GameTypes::RobotType::Basic);
        wave.robotCounts.push_back(composition.basicRobots);
    }

    if (composition.fireRobots > 0) {
        wave.robotTypes.push_back(GameTypes::RobotType::Fire);
        wave.robotCounts.push_back(composition.fireRobots);
    }

    if (composition.stealthRobots > 0) {
        wave.robotTypes.push_back(GameTypes::RobotType::Stealth);
        wave.robotCounts.push_back(composition.stealthRobots);
    }

    // Use randomized spawn order
    wave.spawnOrder = composition.spawnOrder;
    return wave;
}
// Get total enemy count for current wave
int WaveManager::getCurrentWaveEnemyCount() const {
    if (m_currentWaveIndex >= m_waves.size()) return 0;

    const WaveData& wave = m_waves[m_currentWaveIndex];

    // Use spawn order if available (preferred method)
    if (!wave.spawnOrder.empty()) {
        return static_cast<int>(wave.spawnOrder.size());
    }

    // Fallback: sum robot counts
    int total = 0;
    for (int count : wave.robotCounts) {
        total += count;
    }
    return total;
}
// Setup fonts and text objects for countdown
void WaveManager::setupCountdownText() {
    if (m_countdownTextSetup) return;

    try {
        // Try to load primary font
        auto& font = ResourceManager::getInstance().getFont("bruce");

        // Setup countdown number text (large, bold)
        m_countdownText.setFont(font);
        m_countdownText.setCharacterSize(64);
        m_countdownText.setStyle(sf::Text::Bold);

        // Setup wave text (medium, bold)
        m_waveText.setFont(font);
        m_waveText.setCharacterSize(48);
        m_waveText.setStyle(sf::Text::Bold);

        m_countdownTextSetup = true;
    }
    catch (...) {
        // Fallback to system font
        try {
            auto& fallbackFont = ResourceManager::getInstance().getFont("DejaVuSans.ttf");

            m_countdownText.setFont(fallbackFont);
            m_countdownText.setCharacterSize(64);
            m_countdownText.setStyle(sf::Text::Bold);

            m_waveText.setFont(fallbackFont);
            m_waveText.setCharacterSize(48);
            m_waveText.setStyle(sf::Text::Bold);

            m_countdownTextSetup = true;
        }
        catch (...) {
            // Font setup failed - countdown will not display
        }
    }
}

// Move to next countdown phase
void WaveManager::nextCountdownPhase() {
    switch (m_countdownPhase) {
    case GameTypes::CountdownPhase::GetReady:
        // Move from "GET READY" to wave display with countdown
        m_countdownPhase = GameTypes::CountdownPhase::WaveDisplay;
        m_countdownNumber = 5;
        m_countdownDisplayTimer.setDuration(GameConstants::Countdown::COUNTDOWN_DURATION);
        m_countdownDisplayTimer.restart();
        updateTextForCurrentPhase();
        break;

    case GameTypes::CountdownPhase::WaveDisplay:
        // Countdown from 5 to 0
        m_countdownNumber--;
        if (m_countdownNumber >= 0) {
            m_countdownDisplayTimer.restart();
            updateTextForCurrentPhase();
        }
        else {
            // Countdown finished - ready to start wave
            m_countdownPhase = GameTypes::CountdownPhase::Complete;
        }
        break;

    default:
        break;
    }
}

// Update text content for current countdown phase
void WaveManager::updateTextForCurrentPhase() {
    if (!m_countdownTextSetup) return;

    switch (m_countdownPhase) {
    case GameTypes::CountdownPhase::GetReady:
        m_countdownText.setString("GET READY FOR COMBAT!");
        m_countdownText.setCharacterSize(36);  // Smaller for long text
        m_countdownText.setFillColor(sf::Color::White);
        break;

    case GameTypes::CountdownPhase::WaveDisplay:
        // Set wave text
        m_waveText.setString("WAVE " + std::to_string(getCurrentWave()));
        m_waveText.setFillColor(sf::Color::White);

        // Set countdown number
        if (m_countdownNumber > 0) {
            m_countdownText.setString(std::to_string(m_countdownNumber));
        }
        else {
            m_countdownText.setString("0");
        }
        m_countdownText.setFillColor(sf::Color(255, 215, 0));  // Gold
        break;

    default:
        m_countdownText.setString("");
        m_waveText.setString("");
        break;
    }
}

// Calculate fade alpha for countdown display
float WaveManager::calculateCountdownAlpha() const {
    if (m_countdownPhase == GameTypes::CountdownPhase::None) return 0.0f;

    float elapsed = m_countdownDisplayTimer.getElapsed();
    float duration = m_countdownDisplayTimer.getDuration();
    float fadeIn = GameConstants::Countdown::FADE_DURATION;
    float fadeOut = GameConstants::Countdown::FADE_DURATION;
    float stayDuration = duration - fadeIn - fadeOut;

    // Fade in phase
    if (elapsed <= fadeIn) {
        return (elapsed / fadeIn) * 255.0f;
    }
    // Stay visible phase
    else if (elapsed <= fadeIn + stayDuration) {
        return 255.0f;
    }
    // Fade out phase
    else {
        float fadeOutElapsed = elapsed - fadeIn - stayDuration;
        return std::max(0.0f, (1.0f - (fadeOutElapsed / fadeOut)) * 255.0f);
    }
}