// ================================
// src/UI/HUD.cpp
// ==============================
#include "UI/HUD.h"
#include "Core/GameManager.h"
#include "Entities/Base/SquadMember.h"
#include <iostream>
#include "Managers/ResourceManager.h"
#include "UI/Button.h"

void HUD::setupUI() {
    // Initialize UI elements here when you're ready to implement visuals

    m_healthPackCountText.setFont(ResourceManager::getInstance().getFont("DejaVuSans.ttf"));
    m_healthPackCountText.setCharacterSize(20);
    m_healthPackCountText.setFillColor(sf::Color::White);
    m_healthPackCountText.setStyle(sf::Text::Bold);
}

HUD::HUD() {
    setupUI();
}

void HUD::update(float dt) {
    updateHealthBar();
    updateUnitInfo(dt);
    updateVisualFeedback(dt);
    updateRangeIndicators(dt);
}

void HUD::draw(sf::RenderWindow& window) const {
    // Draw UI elements
    renderUnitInfo(window);
    renderVisualFeedback(window);
    renderRangeIndicators(window);
    renderTargetingVisualization(window);
}

void HUD::handleEvent(const sf::Event& event) {
    // Handle UI events
}

void HUD::setGameManager(GameManager* gameManager) {
    m_gameManager = gameManager;
}

void HUD::updateCoins(int coins) {
    // TODO: Update actual coin display UI element
}

void HUD::updateBaseHealth(int health, int maxHealth) {
    // TODO: Update actual health bar UI element
}

void HUD::updateScore(int score) {
    // TODO: Update actual score display UI element
}

void HUD::updateWaveInfo(int currentWave, int totalWaves) {
    // TODO: Update actual wave display UI element
}

void HUD::updateHealthPackCount(int count) {
    m_currentHealthPackCount = count;
    m_healthPackCountText.setString(std::to_string(count));
}

void HUD::setSelectedUnitType(GameTypes::SquadMemberType type) {
    m_selectedUnitType = type;
}

GameTypes::SquadMemberType HUD::getSelectedUnitType() const {
    return m_selectedUnitType;
}

void HUD::updateUnitCount(int count) {
    // TODO: Update actual unit count display
}

void HUD::notifyUnitPlaced(SquadMember* unit) {
    // CHANGED: Use ParticleSystem instead of local effects
}

void HUD::notifyUnitUpgraded(SquadMember* unit) {
    // CHANGED: Use ParticleSystem instead of local effects

}

void HUD::notifyUnitSold(SquadMember* unit) {
    // CHANGED: Use ParticleSystem for sell effect

}

void HUD::notifyUnitSelected(SquadMember* unit) {
    // TODO: Highlight selected unit
}

void HUD::showHealthPackUsage(SquadMember* target) {

}

// Unit visual feedback methods
void HUD::showUnitInfo(SquadMember* unit) {
    m_selectedUnit = unit;
    m_showingUnitInfo = true;
}

void HUD::hideUnitInfo() {
    m_showingUnitInfo = false;
    m_selectedUnit = nullptr;
}

void HUD::showUnitStats(SquadMember* unit) {
    // TODO: Implement unit stats display
}

void HUD::showUpgradePreview(SquadMember* unit) {
    // TODO: Implement upgrade preview
}

void HUD::showRangePreview(GameTypes::SquadMemberType type, const sf::Vector2f& position) {
    m_showingRangePreview = true;
    m_rangePreview.setPosition(position);
    // Set range based on unit type - you'll need to get this from config
    m_rangePreview.setRadius(200.0f); // Default range
}

void HUD::hideRangePreview() {
    m_showingRangePreview = false;
}

void HUD::showRangeIndicator(SquadMember* unit, bool show) {
    if (show) {
        m_unitRangeIndicators[unit].setPosition(unit->getPosition());
        m_unitRangeIndicators[unit].setRadius(unit->getRange());
    }
    else {
        m_unitRangeIndicators.erase(unit);
    }
}

void HUD::showAllRangeIndicators(bool show) {
    m_showRangeIndicators = show;
}

void HUD::updateRangeIndicator(SquadMember* unit) {
    if (m_unitRangeIndicators.find(unit) != m_unitRangeIndicators.end()) {
        m_unitRangeIndicators[unit].setPosition(unit->getPosition());
        m_unitRangeIndicators[unit].setRadius(unit->getRange());
    }
}

void HUD::highlightSelectedUnit(SquadMember* unit) {
    m_selectedUnit = unit;
    if (unit) {
        m_unitHighlight.setPosition(unit->getPosition());
        m_unitHighlight.setSize(sf::Vector2f(50, 50));
    }
}

void HUD::clearUnitHighlight() {
    m_selectedUnit = nullptr;
}

void HUD::clearHighlights() {
    clearUnitHighlight();
    hideRangePreview();
    m_showingPlacementGrid = false;
}

void HUD::showTargetingLine(SquadMember* unit, const sf::Vector2f& targetPos) {
    if (unit) {
        m_targetingLine[0] = sf::Vertex(unit->getPosition(), sf::Color::Red);
        m_targetingLine[1] = sf::Vertex(targetPos, sf::Color::Red);
        m_showTargetingLine = true;
    }
}

void HUD::hideTargetingLine() {
    m_showTargetingLine = false;
}

// Private helper methods
void HUD::updateHealthBar() {
    // TODO: Implement actual health bar update
}

void HUD::updateUnitInfo(float dt) {
    // TODO: Implement unit info panel updates
}

void HUD::updateVisualFeedback(float dt) {
    // TODO: Implement visual feedback updates
}

void HUD::updateRangeIndicators(float dt) {
    // TODO: Implement range indicator updates
}

void HUD::onUnitButtonClicked(GameTypes::SquadMemberType type) {
    setSelectedUnitType(type);
}

void HUD::renderUnitInfo(sf::RenderWindow& window) const {
    // TODO: Implement unit info rendering
}

void HUD::renderVisualFeedback(sf::RenderWindow& window) const {
    if (m_selectedUnit) {
        window.draw(m_unitHighlight);
    }

    if (m_showingRangePreview) {
        window.draw(m_rangePreview);
    }
}

void HUD::renderRangeIndicators(sf::RenderWindow& window) const {
    if (m_showRangeIndicators) {
        for (const auto& [unit, indicator] : m_unitRangeIndicators) {
            window.draw(indicator);
        }
    }
}

void HUD::renderTargetingVisualization(sf::RenderWindow& window) const {
    if (m_showTargetingLine) {
        window.draw(m_targetingLine, 2, sf::Lines);
    }
}