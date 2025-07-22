#pragma once
#include "Core/Constants.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class HighScoreManager {
public:
    // Represents a single high score entry
    struct HighScore {
        int score;
        int wave;
        float time;
        std::string date;
        std::string playerName = "Player";
    };

    static HighScoreManager& getInstance(); // Singleton access

    // High score operations
    bool saveHighScore(const HighScore& score);              // Save new high score
    std::vector<HighScore> loadHighScores() const;           // Load all high scores
    bool isNewHighScore(int score) const;                    // Check if score qualifies
    int getHighScore() const;                                // Get highest score
    int getLowestHighScore() const;                          // Get lowest high score

    // High score queries
    std::vector<HighScore> getTopScores(int count = 10) const; // Get top N scores
    int getPlayerRank(int score) const;                       // Get rank of a score
    bool isScoreInTopTen(int score) const;                    // Check if in top 10

    // Game over utilities
    std::string getReasonText(int reasonValue) const;         // Text for game over reason
    sf::Color getReasonColor(int reasonValue) const;          // Color for reason

    // High score management
    void clearHighScores();                                   // Clear all high scores
    bool exportHighScores(const std::string& filename) const; // Export to file
    bool importHighScores(const std::string& filename);       // Import from file

    // Configuration
    void setMaxHighScores(int maxScores);                     // Set max scores limit
    int getMaxHighScores() const;                             // Get max scores limit

    // Error handling
    std::string getLastError() const;                         // Retrieve last error
    bool hasErrors() const;                                   // Check if error exists

private:
    HighScoreManager() = default;
    ~HighScoreManager() = default;
    HighScoreManager(const HighScoreManager&) = delete;
    HighScoreManager& operator=(const HighScoreManager&) = delete;

    int m_maxHighScores = 10;            // Maximum stored scores
    mutable std::string m_lastError;     // Last error message
    mutable std::vector<HighScore> m_cachedScores; // Cached scores
    mutable bool m_cacheValid = false;   // Is cache valid

    bool writeToFile(const std::string& filename, const std::string& data);      // Write to file
    bool readFromFile(const std::string& filename, std::string& data);           // Read from file
    std::string serializeHighScores(const std::vector<HighScore>& scores);       // Serialize scores
    bool deserializeHighScores(const std::string& data, std::vector<HighScore>& scores); // Deserialize
    void sortHighScores(std::vector<HighScore>& scores) const;                   // Sort scores
    void trimToMaxScores(std::vector<HighScore>& scores) const;                  // Enforce max limit
    void invalidateCache() const;                                                // Invalidate cache
    std::string getCurrentDateString() const;                                    // Current date as string
    std::string getHighScoreFilePath() const;                                    // Path to save scores
    void setError(const std::string& error) const;                               // Set error message
    void clearError() const;                                                     // Clear error
};
