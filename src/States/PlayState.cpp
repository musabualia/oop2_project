// ================================
// src/States/PlayState.cpp - Main Gameplay State Implementation
// Handles all gameplay mechanics, UI interactions, and game progression
// ================================

#include "States/PlayState.h"
#include "Core/GameManager.h"
#include "Core/Game.h"
#include "States/StateMachine.h"
#include "States/LevelsState.h"
#include "States/GameOverState.h"
#include "States/VictoryState.h"
#include "States/PauseState.h"
#include "Entities/Base/SquadMember.h"
#include "Managers/ResourceManager.h"
#include "Managers/AudioManager.h"
#include "Managers/SquadMemberManager.h"
#include "Managers/WaveManager.h"
#include "Factories/CollectibleFactory.h"
#include "Systems/AnimationSystem.h"
#include "Systems/EventSystem.h"
#include "Commands/PlaceUnitCommand.h"
#include "Commands/PlaceBombCommand.h"
#include "UI/GridRenderer.h"
#include "UI/HUD.h"
#include "Utils/ConfigLoader.h"
#include <memory>

// Constructor - Initialize game systems and load resources
PlayState::PlayState(Game* game, StateMachine* machine, int level)
    : m_game(game), m_machine(machine), m_currentLevel(level) {

    // Create core game systems
    m_gameManager = std::make_unique<GameManager>();
    m_gameManager->setCurrentLevel(m_currentLevel);
    m_gridRenderer = std::make_unique<GridRenderer>();
    m_commandManager = std::make_unique<CommandManager>();
    m_commandManager->setMaxHistory(50);

    // Setup message timers
    m_notEnoughCoinsTimer.setDuration(3.0f);

    // Load unit textures - sniper locked until level 9
    auto& rm = ResourceManager::getInstance();
    m_unitTextures = {
        rm.getTexture("select1.png"),
        rm.getTexture("select2.png"),
        (m_currentLevel >= 9) ? rm.getTexture("select3.png") : rm.getTexture("Sniper_Locked.png")
    };

    m_lockedMessageTexture = rm.getTexture("Locked_Message.png");
    m_healthPackTexture = rm.getTexture("select_medkit.png");

    // Load unit costs from config
    auto& config = ConfigLoader::getInstance();
    config.loadFromFile("units.cfg");
    m_unitCosts = {
        config.getUnitCost(GameTypes::SquadMemberType::HeavyGunner),
        config.getUnitCost(GameTypes::SquadMemberType::ShieldBearer),
        config.getUnitCost(GameTypes::SquadMemberType::Sniper)
    };
}
// Main event handler - processes all user input
void PlayState::handleEvent(const sf::Event& event) {
    // Handle keyboard shortcuts
    if (event.type == sf::Event::KeyPressed) {
        // Undo command (Ctrl + Z)
        if (event.key.code == sf::Keyboard::Z && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
            if (m_commandManager->canUndo()) {
                m_commandManager->undo();
            }
            return;
        }
        // Redo command (Ctrl + Y)
        if (event.key.code == sf::Keyboard::Y && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
            if (m_commandManager->canRedo()) {
                m_commandManager->redo();
            }
            return;
        }
        // Exit to level select (Escape)
        if (event.key.code == sf::Keyboard::Escape) {
            if (m_machine && m_game) {
                if (!m_paused && m_gameManager)
                    m_gameManager->pauseGame();
                m_machine->changeState(std::make_unique<LevelsState>(m_game, m_machine));
            }
            return;
        }
        // Pause game (Space)
        if (event.key.code == sf::Keyboard::Space) {
            if (m_machine && m_game && !m_paused) {
                m_gameManager->pauseGame();
                onPause();
                m_machine->pushState(std::make_unique<PauseState>(m_game, m_machine));
            }
            return;
        }
    }
    // Handle mouse input
    if (event.type == sf::Event::MouseButtonPressed ||
        event.type == sf::Event::MouseButtonReleased) {
        handleMouseEvents(event);
    }
    // Handle mouse movement for hover detection
    if (event.type == sf::Event::MouseMoved) {
        handleMouseHover(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
    }
    // Pass other events to game manager
    if (m_gameManager && !m_paused) {
        m_gameManager->handleEvent(event);
    }
}
// Handle mouse hover for locked sniper message
void PlayState::handleMouseHover(const sf::Vector2f& mousePos) {
    bool wasHovering = m_isHoveringLockedSniper;
    m_isHoveringLockedSniper = false;
    // Check if hovering over locked sniper icon (only before level 9)
    if (m_currentLevel < 9 && m_unitIconBounds.size() > 2) {
        if (m_unitIconBounds[2].contains(mousePos)) {
            m_isHoveringLockedSniper = true;
        }
    }
    // Play hover sound when starting to hover
    if (!wasHovering && m_isHoveringLockedSniper) {
        AudioManager::getInstance().playSound("button_hover", AudioManager::AudioCategory::UI, 0.5f);
    }
}
// Handle mouse clicks on UI elements
void PlayState::handleMouseEvents(const sf::Event& event) {
    if (event.type != sf::Event::MouseButtonPressed ||
        event.mouseButton.button != sf::Mouse::Left) return;
    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
    // Check undo icon click
    if (m_undoIconBounds.contains(mousePos)) {
        if (m_commandManager->canUndo()) {
            m_commandManager->undo();
        }
        else {
            AudioManager::getInstance().playSound("error", AudioManager::AudioCategory::UI);
        }
        return;
    }
    // Check redo icon click
    if (m_redoIconBounds.contains(mousePos)) {
        if (m_commandManager->canRedo()) {
            m_commandManager->redo();
        }
        else {
            AudioManager::getInstance().playSound("error", AudioManager::AudioCategory::UI);
        }
        return;
    }
    // Check level select icon
    if (m_levelSelectBounds.contains(mousePos)) {
        if (m_machine && m_game) {
            m_machine->changeState(std::make_unique<LevelsState>(m_game, m_machine));
        }
        return;
    }
    // Check unit selection icons
    for (int i = 0; i < m_unitIconBounds.size() && i < 3; ++i) {
        if (m_unitIconBounds[i].contains(mousePos)) {
            // Block sniper selection for early levels
            if (i == 2 && m_currentLevel < 9) {
                return; // No action for locked sniper
            }

            // Reset special modes and select unit
            m_bombMode = false;
            m_healthPackMode = false;

            switch (i) {
            case 0: m_selectedUnitType = GameTypes::SquadMemberType::HeavyGunner; break;
            case 1: m_selectedUnitType = GameTypes::SquadMemberType::ShieldBearer; break;
            case 2:
                if (m_currentLevel >= 9) {
                    m_selectedUnitType = GameTypes::SquadMemberType::Sniper;
                }
                break;
            }
            return;
        }
    }

    // Check bomb icon
    if (m_bombIconBounds.contains(mousePos)) {
        m_bombMode = true;
        m_healthPackMode = false;
        return;
    }
    // Check HealthPack icon
    if (m_healthPackIconBounds.contains(mousePos)) {
        if (m_gameManager && m_gameManager->getHealthPackCount() > 0) {
            m_healthPackMode = true;
            m_bombMode = false;
        }
        return;
    }
    // Handle placement on game grid
    handleObjectPlacement(mousePos);
}
// Handle object placement on the game grid
void PlayState::handleObjectPlacement(const sf::Vector2f& mousePos) {
    AudioManager::getInstance().playSound("placed", AudioManager::AudioCategory::UI);
    if (m_healthPackMode) {
        handleHealthPackUsage(mousePos);
    }
    else if (m_bombMode) {
        handleBombPlacement(mousePos);
    }
    else {
        handleSquadMemberPlacement(mousePos);
    }
}
// Handle bomb placement using command pattern
void PlayState::handleBombPlacement(const sf::Vector2f& mousePos) {
    sf::Vector2i gridPos = m_gridRenderer->getGridPosition(mousePos);
    if (!m_gridRenderer->isValidPlacement(gridPos)) {
        return;
    }
    const int BOMB_COST = 100;
    if (!m_gameManager->spendCoins(BOMB_COST)) {
        m_showNotEnoughCoinsMessage = true;
        m_notEnoughCoinsTimer.restart();
        return;
    }
    // Calculate world position from grid position
    sf::Vector2f worldPos = m_gridRenderer->gridToWorldPosition(gridPos.x, gridPos.y);
    worldPos += sf::Vector2f(GameConstants::GRID_CELL_WIDTH / 2.f, GameConstants::GRID_CELL_HEIGHT / 2.f);

    // Create and execute bomb placement command
    auto command = std::make_unique<PlaceBombCommand>(&m_bombs, worldPos);
    m_commandManager->executeCommand(std::move(command));
}
// Handle squad member placement using command pattern
void PlayState::handleSquadMemberPlacement(const sf::Vector2f& mousePos) {
    // Block sniper placement in early levels
    if (m_selectedUnitType == GameTypes::SquadMemberType::Sniper && m_currentLevel < 9) {
        return;
    }

    sf::Vector2i gridPos = m_gridRenderer->getGridPosition(mousePos);
    if (!m_gridRenderer->isValidPlacement(gridPos)) return;

    int cost = m_unitCosts[static_cast<int>(m_selectedUnitType)];
    int coins = m_gameManager->getCoins();

    if (coins >= cost) {
        // Create and execute unit placement command
        auto command = std::make_unique<PlaceUnitCommand>(
            m_gameManager.get(),
            m_selectedUnitType,
            gridPos.y,  // lane
            gridPos.x,  // gridX
            m_gridRenderer.get()
        );

        m_commandManager->executeCommand(std::move(command));
    }
    else {
        // Show insufficient coins message
        m_showNotEnoughCoinsMessage = true;
        m_notEnoughCoinsTimer.restart();
    }
}
// Handle health pack usage on squad members
void PlayState::handleHealthPackUsage(const sf::Vector2f& mousePos) {
    if (!m_gameManager) return;

    sf::Vector2i gridPos = m_gridRenderer->getGridPosition(mousePos);
    if (!m_gridRenderer->isValidGridPosition(gridPos.x, gridPos.y)) {
        return;
    }
    // Find squad member at position
    SquadMember* unit = m_gameManager->getSquadMemberManager().getUnitAt(gridPos.y, gridPos.x);
    if (!unit || !unit->canBeHealed()) {
        return;
    }
    // Use health pack if available
    if (m_gameManager->useHealthPack()) {
        auto& factory = CollectibleFactory::getInstance();
        float healPercentage = static_cast<float>(factory.getHealthPackHealPercentage());
        unit->healByPercentage(healPercentage);
        m_gameManager->getHUD().showHealthPackUsage(unit);

        m_healthPackMode = false;
    }
}
// Update game state and animations
void PlayState::update(float dt) {
    if (m_paused) return;

    // Update smooth alpha transition for locked message
    float targetAlpha = m_isHoveringLockedSniper ? 255.0f : 0.0f;

    if (m_lockedMessageAlpha < targetAlpha) {
        m_lockedMessageAlpha = std::min(targetAlpha, m_lockedMessageAlpha + FADE_SPEED * dt);
    }
    else if (m_lockedMessageAlpha > targetAlpha) {
        m_lockedMessageAlpha = std::max(targetAlpha, m_lockedMessageAlpha - FADE_SPEED * dt);
    }

    // Update "not enough coins" message timer
    if (m_showNotEnoughCoinsMessage) {
        m_notEnoughCoinsTimer.update(dt);
        if (m_notEnoughCoinsTimer.isElapsed()) {
            m_showNotEnoughCoinsMessage = false;
        }
    }
    // Update core game systems
    AnimationSystem::getInstance().update(dt);
    EventSystem::getInstance().processEvents();
    m_gridRenderer->update(dt);

    if (m_gameManager) {
        m_gameManager->update(dt);
        checkGameEndConditions();
    }
    // Update and cleanup bombs
    for (size_t i = 0; i < m_bombs.size(); ) {
        m_bombs[i].update(dt);
        if (m_bombs[i].isDone()) {
            m_bombs.erase(m_bombs.begin() + i);
        }
        else {
            ++i;
        }
    }
}
// Render all game elements
void PlayState::render(sf::RenderWindow& window) {
    if (m_paused) return;

    renderBackground(window);
    renderGameContent(window);
    renderUI(window);
}
// Render background image
void PlayState::renderBackground(sf::RenderWindow& window) {
    sf::Sprite bg;
    bg.setTexture(ResourceManager::getInstance().getTexture("level_bg.png"));

    // Scale background to fit window
    sf::Vector2u winSize = window.getSize();
    sf::Vector2u texSize = bg.getTexture()->getSize();
    bg.setScale(
        static_cast<float>(winSize.x) / texSize.x,
        static_cast<float>(winSize.y) / texSize.y
    );
    window.draw(bg);
}
// Render game content (grid, units, bombs, animations)
void PlayState::renderGameContent(sf::RenderWindow& window) {
    m_gridRenderer->draw(window);

    if (m_gameManager) {
        m_gameManager->render(window);
    }

    AnimationSystem::getInstance().render(window);

    // Render active bombs
    for (const auto& bomb : m_bombs) {
        bomb.render(window);
    }
}
// Render all UI elements
void PlayState::renderUI(sf::RenderWindow& window) {
    renderUnitSelectionIcons(window);
    renderCommandIcons(window);
    renderSpecialIcons(window);
    renderHealthPackIcon(window);
    renderCoinDisplay(window);
    renderLevelSelect(window);
    // Render locked message (handles its own visibility)
    renderLockedMessage(window);

    // Render "not enough coins" message with fade effect
    if (m_showNotEnoughCoinsMessage) {
        float elapsedTime = m_notEnoughCoinsTimer.getElapsed();
        float alpha = calculateMessageAlpha(elapsedTime);

        sf::Color textColor = m_notEnoughCoinsText.getFillColor();
        textColor.a = static_cast<sf::Uint8>(alpha);
        m_notEnoughCoinsText.setFillColor(textColor);

        window.draw(m_notEnoughCoinsText);
    }

    // Render wave countdown
    if (m_gameManager) {
        m_gameManager->getWaveManager().renderCountdown(window);
    }
}
// Render unit selection icons at bottom of screen
void PlayState::renderUnitSelectionIcons(sf::RenderWindow& window) {
    sf::Vector2u winSize = window.getSize();
    float iconSize = 130.f;
    float spacing = 15.f;
    float startX = 10.f;
    float startY = winSize.y - iconSize - 10.f;

    m_unitIconBounds.clear();

    for (int i = 0; i < 3; ++i) {
        sf::Sprite icon;
        icon.setTexture(m_unitTextures[i]);
        icon.setScale(
            iconSize / m_unitTextures[i].getSize().x,
            iconSize / m_unitTextures[i].getSize().y
        );
        icon.setPosition(startX + i * (iconSize + spacing), startY);
        m_unitIconBounds.push_back(icon.getGlobalBounds());

        // Gray out locked sniper
        if (i == 2 && m_currentLevel < 9) {
            icon.setColor(sf::Color(180, 180, 180, 255));
        }

        window.draw(icon);
    }
}
// Render bomb icon
void PlayState::renderSpecialIcons(sf::RenderWindow& window) {
    sf::Vector2u winSize = window.getSize();
    float iconSize = 120.f;

    sf::Sprite bombIcon;
    bombIcon.setTexture(ResourceManager::getInstance().getTexture("select-b.png"));
    bombIcon.setScale(
        iconSize / bombIcon.getTexture()->getSize().x,
        iconSize / bombIcon.getTexture()->getSize().y
    );

    sf::Vector2f pos(winSize.x - iconSize - 20.f, winSize.y - iconSize - 20.f);
    bombIcon.setPosition(pos);
    m_bombIconBounds = bombIcon.getGlobalBounds();

    window.draw(bombIcon);
}
// Render health pack icon with counter
void PlayState::renderHealthPackIcon(sf::RenderWindow& window) {
    sf::Vector2u winSize = window.getSize();
    float iconSize = 130.f;
    float spacing = 15.f;
    sf::Vector2f bombPos(winSize.x - iconSize - 20.f, winSize.y - iconSize - 20.f);
    sf::Vector2f healthPos(bombPos.x - iconSize - spacing + 15.f, bombPos.y + 10.f);

    sf::Sprite healthIcon;
    healthIcon.setTexture(m_healthPackTexture);
    healthIcon.setScale(iconSize / m_healthPackTexture.getSize().x, iconSize / m_healthPackTexture.getSize().y);
    healthIcon.setPosition(healthPos);

    m_healthPackIconBounds = healthIcon.getGlobalBounds();
    // Highlight when in health pack mode
    if (m_healthPackMode) {
        healthIcon.setColor(sf::Color(255, 255, 150));
    }
    window.draw(healthIcon);
    // Render health pack counter
    if (m_gameManager) {
        sf::Text counter;
        counter.setFont(ResourceManager::getInstance().getFont("bruce"));
        counter.setCharacterSize(15);
        counter.setFillColor(sf::Color::White);
        counter.setString(std::to_string(m_gameManager->getHealthPackCount()));
        counter.setPosition(healthPos.x + iconSize / 2 - 6, healthPos.y + iconSize * 0.8f - 8.0f);
        window.draw(counter);
    }
}
// Render coin display with current amount
void PlayState::renderCoinDisplay(sf::RenderWindow& window) {
    const int coins = m_gameManager ? m_gameManager->getCoins() : 0;

    window.draw(m_coinUISprite);
    sf::Text coinText;
    coinText.setFont(ResourceManager::getInstance().getFont("bruce"));
    coinText.setCharacterSize(25);
    coinText.setFillColor(sf::Color::White);
    coinText.setStyle(sf::Text::Bold);
    coinText.setString(std::to_string(coins));

    // Position text on coin UI
    sf::FloatRect spriteBounds = m_coinUISprite.getGlobalBounds();
    sf::FloatRect textBounds = coinText.getLocalBounds();

    float barX = spriteBounds.left + spriteBounds.width * 0.65f;
    float barY = spriteBounds.top + spriteBounds.height * 0.49f;

    coinText.setPosition(
        barX - textBounds.width / 2.f,
        barY - textBounds.height / 2.f
    );

    window.draw(coinText);
}
// Render undo/redo command icons
void PlayState::renderCommandIcons(sf::RenderWindow& window) {
    sf::Vector2u winSize = window.getSize();
    float iconSize = 90.f;
    float spacing = 15.f;

    float unitIconY = winSize.y - 130.f - 10.f;
    float commandIconY = unitIconY - iconSize - 10.f;

    // Undo icon
    sf::Sprite undoIcon;
    undoIcon.setTexture(m_undoTexture);
    undoIcon.setScale(iconSize / m_undoTexture.getSize().x, iconSize / m_undoTexture.getSize().y);
    undoIcon.setPosition(30.f, commandIconY);

    // Gray out if unavailable
    if (!m_commandManager->canUndo()) {
        undoIcon.setColor(sf::Color(100, 100, 100, 150));
    }
    m_undoIconBounds = undoIcon.getGlobalBounds();
    window.draw(undoIcon);

    // Redo icon
    sf::Sprite redoIcon;
    redoIcon.setTexture(m_redoTexture);
    redoIcon.setScale(iconSize / m_redoTexture.getSize().x, iconSize / m_redoTexture.getSize().y);
    redoIcon.setPosition(30.f + 130.f + spacing, commandIconY);

    // Gray out if unavailable
    if (!m_commandManager->canRedo()) {
        redoIcon.setColor(sf::Color(100, 100, 100, 150));
    }
    m_redoIconBounds = redoIcon.getGlobalBounds();
    window.draw(redoIcon);
}
// Render level select button and current level display
void PlayState::renderLevelSelect(sf::RenderWindow& window) {
    window.draw(m_levelSelectSprite);

    sf::Text levelText;
    levelText.setFont(ResourceManager::getInstance().getFont("bruce"));
    levelText.setCharacterSize(20);
    levelText.setFillColor(sf::Color::White);
    levelText.setStyle(sf::Text::Bold);
    levelText.setString("Level " + std::to_string(m_currentLevel));

    sf::FloatRect spriteBounds = m_levelSelectSprite.getGlobalBounds();
    sf::FloatRect textBounds = levelText.getLocalBounds();

    levelText.setPosition(
        spriteBounds.left + spriteBounds.width + 160.f,
        spriteBounds.top + (spriteBounds.height - textBounds.height) / 2.f
    );

    window.draw(levelText);
}
// Render locked sniper message with smooth fade effect
void PlayState::renderLockedMessage(sf::RenderWindow& window) {
    // Only render if there's some alpha (visible)
    if (m_lockedMessageAlpha <= 0.0f) return;

    sf::Vector2u winSize = window.getSize();

    float iconSize = 130.f;
    float spacing = 15.f;
    float startX = 10.f;
    float sniperIconX = startX + 2 * (iconSize + spacing);

    sf::Sprite lockedMessage;
    lockedMessage.setTexture(m_lockedMessageTexture);

    // Scale and position message above sniper icon
    float messageWidth = 150.f;
    float messageHeight = 75.f;
    lockedMessage.setScale(
        messageWidth / m_lockedMessageTexture.getSize().x,
        messageHeight / m_lockedMessageTexture.getSize().y
    );

    lockedMessage.setPosition(
        sniperIconX + (iconSize - messageWidth) / 2.f,
        winSize.y - iconSize - messageHeight - 5.f
    );

    // Apply smooth alpha transition
    lockedMessage.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(m_lockedMessageAlpha)));
    window.draw(lockedMessage);
}
// Calculate alpha for "not enough coins" message fade effect
float PlayState::calculateMessageAlpha(float elapsedTime) const {
    const float fadeInDuration = 0.5f;
    const float stayDuration = 4.0f;
    const float fadeOutDuration = 0.5f;

    if (elapsedTime <= fadeInDuration) {
        return (elapsedTime / fadeInDuration) * 255.0f;
    }
    else if (elapsedTime <= fadeInDuration + stayDuration) {
        return 255.0f;
    }
    else {
        float fadeOutElapsed = elapsedTime - fadeInDuration - stayDuration;
        return (1.0f - (fadeOutElapsed / fadeOutDuration)) * 255.0f;
    }
}

void PlayState::onEnter() {
    m_coinUITexture = ResourceManager::getInstance().getTexture("CoinUI.png");
    m_coinUISprite.setTexture(m_coinUITexture);
    m_coinUISprite.setScale(0.35f, 0.35f);
    m_coinUISprite.setPosition(10.f, 10.f);

    auto& rm = ResourceManager::getInstance();
    m_undoTexture = rm.getTexture("undo.png");
    m_redoTexture = rm.getTexture("redo.png");
    m_levelSelectTexture = rm.getTexture("select_level.png");

    m_levelSelectSprite.setTexture(m_levelSelectTexture);
    m_levelSelectSprite.setScale(0.30f, 0.20f);

    sf::Vector2u winSize = m_game->getWindow().getSize();
    m_levelSelectSprite.setPosition(winSize.x - 490.f, 50.f);
    m_levelSelectBounds = m_levelSelectSprite.getGlobalBounds();

    // Setup not enough coins text
    m_notEnoughCoinsText.setFont(ResourceManager::getInstance().getFont("bruce"));
    m_notEnoughCoinsText.setString("Not Enough Coins!");
    m_notEnoughCoinsText.setCharacterSize(48);
    m_notEnoughCoinsText.setStyle(sf::Text::Bold);
    m_notEnoughCoinsText.setFillColor(sf::Color::Red);

    sf::FloatRect textBounds = m_notEnoughCoinsText.getLocalBounds();
    m_notEnoughCoinsText.setPosition(
        (winSize.x - textBounds.width) / 2.f,
        winSize.y * 0.3f
    );
    AnimationSystem::getInstance().initialize();
    if (m_gameManager) {
        m_gameManager->getSquadMemberManager().setGridRenderer(m_gridRenderer.get());

        if (!m_initialized) {
            startNewGame();
            m_initialized = true;
        }
    }
}
// Cleanup when exiting state
void PlayState::onExit() {
    EventSystem::getInstance().clear();
}
// Handle pause state
void PlayState::onPause() {
    m_paused = true;
    if (m_gameManager) m_gameManager->pauseGame();
}
// Handle resume from pause
void PlayState::onResume() {
    m_paused = false;
    if (m_gameManager) m_gameManager->resumeGame();
}
// Check if state is paused
bool PlayState::isPaused() const {
    return m_paused;
}
// Check for game end conditions (victory or defeat)
void PlayState::checkGameEndConditions() {
    if (!m_gameManager) return;

    int finalScore = m_gameManager->getScore();
    int wavesCompleted = m_gameManager->getCurrentWave();
    float timeElapsed = m_gameManager->getElapsedTime();
    int currentLevel = m_currentLevel;

    // Check for game over condition
    if (m_gameManager->isGameOver()) {
        GameOverState::Reason reason = GameOverState::Reason::BaseDestroyed;
        if (m_gameManager->getWavesCompleted() <= 2) {
            reason = GameOverState::Reason::BaseDestroyed;
        }
        m_machine->changeState(std::make_unique<GameOverState>(
            m_game, m_machine, reason,
            finalScore, wavesCompleted, timeElapsed, currentLevel
        ));
        return;
    }
    // Check for victory condition
    if (m_gameManager->isVictory()) {
        bool perfectVictory = timeElapsed < 300.0f; // Perfect if under 5 minutes

        m_machine->changeState(std::make_unique<VictoryState>(
            m_game, m_machine,
            finalScore, wavesCompleted, timeElapsed,
            currentLevel, perfectVictory
        ));
    }
}
// Initialize new game session
void PlayState::startNewGame() {
    if (m_gameManager) {
        m_gameManager->startNewGame();

        // Clear command history for fresh start
        if (m_commandManager) {
            m_commandManager->clear();
        }
    }
}