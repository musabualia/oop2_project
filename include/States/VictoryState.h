// ================================
// Victory State - State Pattern with Physics-Based Celebration Effects
// ================================
#pragma once

#include "States/IState.h"
#include "UI/Button.h"
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <memory>
#include <vector>

class Game;
class StateMachine;

// Victory celebration state with animated confetti and score calculation
class VictoryState : public IState {
public:
    // Constructor with complete victory statistics
    VictoryState(Game* game, StateMachine* machine,
        int finalScore, int wavesCompleted, float timeElapsed,
        int currentLevel = 1, bool perfectVictory = false);

    ~VictoryState() override = default;

    // Core state interface implementation
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

    // State lifecycle management
    void onEnter() override;
    void onExit() override;

private:
    // Core system references
    Game* m_game = nullptr;
    StateMachine* m_machine = nullptr;

    // Victory Statistics
    int m_finalScore = 0;                                    // Base score from gameplay
    int m_wavesCompleted = 0;                                // Number of waves defeated
    int m_currentLevel = 1;                                  // Level completed
    float m_timeElapsed = 0.f;                               // Time taken to complete
    bool m_perfectVictory = false;                           // No damage taken flag
    bool m_newHighScore = false;                             // New high score achieved

    // Score Calculation
    int m_bonusScore = 0;                                    // Bonus points for performance
    int m_totalScore = 0;                                    // Final calculated score

    // UI Elements
    sf::Sprite m_backgroundSprite;                           // Victory background
    sf::Text m_statsText;                                    // Score and statistics display
    sf::Text m_tipText;                                      // Encouragement or tips
    std::vector<std::unique_ptr<Button>> m_buttons;          // Action buttons

    // Physics-Based Confetti System
    std::unique_ptr<b2World> m_world;                        // Box2D world for confetti physics
    std::vector<b2Body*> m_confetti;                         // Confetti particle bodies

    // Internal Setup Methods
    void setupUI();                                          // Initialize UI elements and layout
    void setupBox2D();                                       // Initialize physics world for confetti
    void calculateScores();                                  // Calculate bonus scores and totals
    void checkForNewHighScore();                             // Check and save high score

    // Button Action Handlers
    void onContinue();                                       // Continue to next level
    void onReplay();                                         // Replay current level
    void onMainMenu();                                       // Return to main menu
};