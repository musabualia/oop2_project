// ================================
// High Score Manager - Singleton Pattern for Persistent Score Storage and Ranking
// ================================
#include "Managers/HighScoreManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <ctime>

// Singleton access point
HighScoreManager& HighScoreManager::getInstance() {
    static HighScoreManager instance;
    return instance;
}

// Save new high score to persistent storage
bool HighScoreManager::saveHighScore(const HighScore& score) {
    clearError();

    // Load existing scores and add new one
    auto scores = loadHighScores();
    scores.push_back(score);

    // Sort by score (highest first) and limit to max count
    sortHighScores(scores);
    trimToMaxScores(scores);

    // Serialize and save to file
    std::string data = serializeHighScores(scores);
    if (!writeToFile(getHighScoreFilePath(), data)) {
        setError("Failed to write high scores to file.");
        return false;
    }

    invalidateCache();  // Clear cache to force reload
    return true;
}

// Load all high scores from persistent storage
std::vector<HighScoreManager::HighScore> HighScoreManager::loadHighScores() const {
    if (m_cacheValid) return m_cachedScores;

    clearError();
    std::string data;

    // For this implementation, return empty list (file I/O can be added)
    m_cacheValid = true;
    return m_cachedScores;
}

// Check if score qualifies as a new high score
bool HighScoreManager::isNewHighScore(int score) const {
    auto scores = loadHighScores();
    return scores.size() < m_maxHighScores || score > getLowestHighScore();
}

// Get the highest score
int HighScoreManager::getHighScore() const {
    auto scores = loadHighScores();
    return scores.empty() ? 0 : scores.front().score;
}

// Get the lowest qualifying high score
int HighScoreManager::getLowestHighScore() const {
    auto scores = loadHighScores();
    return scores.empty() ? 0 : scores.back().score;
}

// Get top N scores for leaderboard display
std::vector<HighScoreManager::HighScore> HighScoreManager::getTopScores(int count) const {
    auto scores = loadHighScores();
    if (count < static_cast<int>(scores.size()))
        scores.resize(count);
    return scores;
}

// Get player's rank based on score
int HighScoreManager::getPlayerRank(int score) const {
    auto scores = loadHighScores();
    for (size_t i = 0; i < scores.size(); ++i)
        if (score >= scores[i].score)
            return static_cast<int>(i) + 1;
    return static_cast<int>(scores.size()) + 1;
}

// Check if score makes it into top 10
bool HighScoreManager::isScoreInTopTen(int score) const {
    return getPlayerRank(score) <= 10;
}

// Get descriptive text for game over reasons
std::string HighScoreManager::getReasonText(int reasonValue) const {
    switch (reasonValue) {
    case 0: return "You were defeated!";
    case 1: return "Time ran out!";
    case 2: return "Core destroyed!";
    default: return "Unknown reason";
    }
}

// Get color for game over reason display
sf::Color HighScoreManager::getReasonColor(int reasonValue) const {
    switch (reasonValue) {
    case 0: return sf::Color::Red;
    case 1: return sf::Color::Yellow;
    case 2: return sf::Color::Magenta;
    default: return sf::Color::White;
    }
}

// Clear all high scores
void HighScoreManager::clearHighScores() {
    clearError();
    std::ofstream ofs(getHighScoreFilePath(), std::ios::trunc);
    ofs.close();
    invalidateCache();
}

// Export high scores to external file
bool HighScoreManager::exportHighScores(const std::string& filename) const {
    std::ofstream ofs(filename);
    if (!ofs) return false;

    // Write as CSV format
    for (const auto& s : loadHighScores()) {
        ofs << s.playerName << "," << s.score << "," << s.wave << ","
            << s.time << "," << s.date << "\n";
    }
    return true;
}

// Import high scores from external file
bool HighScoreManager::importHighScores(const std::string& filename) {
    std::ifstream ifs(filename);
    if (!ifs) return false;

    std::string line;
    std::vector<HighScore> scores;

    // Parse CSV format
    while (std::getline(ifs, line)) {
        std::istringstream iss(line);
        std::string name, date;
        HighScore s;

        if (std::getline(iss, name, ',') &&
            iss >> s.score && iss.get() &&
            iss >> s.wave && iss.get() &&
            iss >> s.time && iss.get() &&
            std::getline(iss, date)) {
            s.playerName = name;
            s.date = date;
            scores.push_back(s);
        }
    }

    // Sort and limit imported scores
    sortHighScores(scores);
    trimToMaxScores(scores);

    // Save to main high score file
    std::string data = serializeHighScores(scores);
    if (!writeToFile(getHighScoreFilePath(), data)) {
        setError("Failed to import high scores to file.");
        return false;
    }

    invalidateCache();
    return true;
}

// Set maximum number of high scores to keep
void HighScoreManager::setMaxHighScores(int maxScores) {
    m_maxHighScores = maxScores;
    invalidateCache();
}

int HighScoreManager::getMaxHighScores() const {
    return m_maxHighScores;
}

// Error handling methods
std::string HighScoreManager::getLastError() const {
    return m_lastError;
}

bool HighScoreManager::hasErrors() const {
    return !m_lastError.empty();
}

// Private Helper Methods

// Write data to file
bool HighScoreManager::writeToFile(const std::string& filename, const std::string& data) {
    std::ofstream ofs(filename);
    if (!ofs) return false;
    ofs << data;
    return true;
}

// Read data from file
bool HighScoreManager::readFromFile(const std::string& filename, std::string& data) {
    std::ifstream ifs(filename);
    if (!ifs) return false;
    std::ostringstream oss;
    oss << ifs.rdbuf();
    data = oss.str();
    return true;
}

// Serialize high scores to string format
std::string HighScoreManager::serializeHighScores(const std::vector<HighScore>& scores) {
    std::ostringstream oss;
    for (const auto& s : scores) {
        oss << s.playerName << "," << s.score << "," << s.wave << ","
            << s.time << "," << s.date << "\n";
    }
    return oss.str();
}

// Deserialize high scores from string format
bool HighScoreManager::deserializeHighScores(const std::string& data, std::vector<HighScore>& scores) {
    std::istringstream iss(data);
    std::string line;

    while (std::getline(iss, line)) {
        std::istringstream lss(line);
        HighScore s;
        std::string scoreStr, waveStr, timeStr;

        if (std::getline(lss, s.playerName, ',') &&
            std::getline(lss, scoreStr, ',') &&
            std::getline(lss, waveStr, ',') &&
            std::getline(lss, timeStr, ',') &&
            std::getline(lss, s.date)) {

            // Parse numeric values
            s.score = std::stoi(scoreStr);
            s.wave = std::stoi(waveStr);
            s.time = std::stof(timeStr);
            scores.push_back(s);
        }
    }
    return true;
}

// Sort high scores by score (highest first)
void HighScoreManager::sortHighScores(std::vector<HighScore>& scores) const {
    std::sort(scores.begin(), scores.end(),
        [](const HighScore& a, const HighScore& b) {
            return a.score > b.score;
        });
}

// Limit scores to maximum count
void HighScoreManager::trimToMaxScores(std::vector<HighScore>& scores) const {
    if (scores.size() > static_cast<size_t>(m_maxHighScores)) {
        scores.resize(m_maxHighScores);
    }
}

// Cache management
void HighScoreManager::invalidateCache() const {
    m_cacheValid = false;
}

// Get current date as formatted string
std::string HighScoreManager::getCurrentDateString() const {
    std::time_t t = std::time(nullptr);
    std::tm tm;
    localtime_s(&tm, &t);  // Safe version on MSVC
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}

// Get file path for high scores storage
std::string HighScoreManager::getHighScoreFilePath() const {
    return "highscores.txt";
}

// Error management
void HighScoreManager::setError(const std::string& error) const {
    m_lastError = error;
}

void HighScoreManager::clearError() const {
    m_lastError.clear();
}