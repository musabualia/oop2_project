// ================================
// Game Over State - State Pattern with Particle Effects for Defeat Scenarios
// ================================
#pragma once

#include "States/IState.h"
#include "UI/Button.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class Game;
class StateMachine;

// Game over state with defeat reason analysis and encouragement system
class GameOverState : public IState {
public:
    // Enumeration of possible defeat reasons
    enum class Reason {
        BaseDestroyed,    // Player base was destroyed by enemies
        TimeUp,           // Time limit exceeded
        PlayerQuit,       // Player manually quit
        AllUnitsLost      // All squad members were defeated
    };

    // Constructor with complete defeat statistics
    GameOverState(Game* game, StateMachine* machine, Reason reason,
        int finalScore, int wavesCompleted,
        float timeElapsed, int currentLevel = 1);

    ~GameOverState() override = default;

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

    // Defeat Information
    Reason m_reason;                                         // Reason for game over
    int m_finalScore = 0;                                    // Final achieved score
    int m_wavesCompleted = 0;                                // Waves completed before defeat
    int m_currentLevel = 1;                                  // Level being attempted
    float m_timeElapsed = 0.0f;                              // Time elapsed before defeat

    // UI Elements
    sf::Sprite m_backgroundSprite;                           // Defeat background
    sf::Text m_statsText;                                    // Statistics display
    sf::Text m_tipText;                                      // Helpful tips and encouragement
    sf::Text m_titleText;                                    // "GAME OVER" title
    std::vector<std::unique_ptr<Button>> m_buttons;          // Action buttons

    // Atmospheric Smoke Particle System
    struct Smoke {
        sf::Vector2f position;                               // Current position
        float speed;                                         // Rising speed
        sf::CircleShape shape;                               // Visual representation
    };
    std::vector<Smoke> m_smokeParticles;                     // Collection of smoke particles

    // Internal Setup Methods
    void setupUI();                                          // Initialize UI elements and layout
    void setupSmoke();                                       // Initialize smoke particle system
    std::string getEncouragementTip() const;                 // Generate helpful tip based on performance

    // Button Action Handlers
    void onRestart();                                        // Restart current level
    void onBackToLevels();                                   // Return to level selection
};