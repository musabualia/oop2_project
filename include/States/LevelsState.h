// ================================
// Levels State - State Pattern with Scrollable Level Selection Interface
// ================================
#pragma once
#include "States/IState.h"
#include <SFML/Graphics.hpp>
#include "UI/Button.h"
#include <vector>
#include <memory>

class StateMachine;
class Game;

// Level selection state with scrollable map interface and level progression
class LevelsState : public IState {
public:
    LevelsState(Game* game, StateMachine* machine);
    ~LevelsState() override = default;

    // Core state interface implementation
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

    // State lifecycle management
    void onEnter() override;
    void onExit() override;

private:
    // Core system references
    Game* m_game;
    StateMachine* m_machine;

    // Level Selection System
    std::vector<std::unique_ptr<Button>> m_levelButtons;     // Clickable level buttons
    std::vector<sf::Vector2f> m_levelPositions;             // Positions for level buttons on map

    // Multi-Part Background System (stitched panoramic background)
    sf::Texture m_leftTexture;                              // Left section of background
    sf::Texture m_middleTexture;                            // Middle section of background
    sf::Texture m_rightTexture;                             // Right section of background
    sf::Sprite m_leftBackground;                            // Left background sprite
    sf::Sprite m_middleBackground;                          // Middle background sprite
    sf::Sprite m_rightBackground;                           // Right background sprite

    // Decorative Elements
    sf::Texture m_warningTapeTexture;                       // Warning tape texture
    sf::Sprite m_warningTapeTop;                            // Top warning tape decoration
    sf::Sprite m_warningTapeBottom;                         // Bottom warning tape decoration

    // Navigation Elements
    sf::Texture m_backToMenuTexture;                        // Back button texture
    sf::Sprite m_backToMenuSprite;                          // Back button sprite
    sf::FloatRect m_backToMenuBounds;                       // Back button collision bounds

    // Scrollable Map View System
    sf::View m_mapView;                                     // Camera view for scrolling
    float m_mapScrollX = 0.0f;                              // Current horizontal scroll position
    float m_maxScrollLeft = 0.0f;                           // Maximum scroll left boundary
    float m_maxScrollRight = 0.0f;                          // Maximum scroll right boundary
    float m_totalMapWidth = 0.0f;                           // Total width of the scrollable map

    // Mouse Drag Scrolling System
    bool m_isDragging = false;                              // Is user currently dragging
    sf::Vector2f m_lastMousePos;                            // Last recorded mouse position
    float m_scrollSpeed = 1.0f;                             // Scroll sensitivity multiplier

    // Setup and Initialization Methods
    void setupUI();                                         // Initialize all UI elements
    void setupLevelButtons();                               // Create and position level buttons
    void setupMapView();                                    // Initialize scrollable view system
    void loadBackgroundImages();                            // Load multi-part background textures
    void setupWarningTape();                                // Position decorative warning tapes
    void setupBackToMenuButton();                           // Initialize back navigation button

    // Scrolling System Methods
    void updateScrolling(const sf::Event& event);           // Handle mouse drag scrolling
    void updateMapView();                                   // Update camera view position
    void clampScrollPosition();                             // Ensure scroll stays within bounds
    void generateLevelPositions();                          // Calculate positions for level buttons

    // Level Selection Handler
    void onLevelSelected(int levelIndex);                   // Handle level button clicks
};