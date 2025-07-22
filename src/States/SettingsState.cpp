// ================================
// Settings State - State Pattern Implementation for Game Configuration Management
// ================================
#include "States/SettingsState.h"
#include "States/MenuState.h"
#include "Managers/ResourceManager.h"
#include "Managers/SettingsManager.h"
#include "Managers/AudioManager.h"
#include "Core/Game.h"
#include "Core/Constants.h"
#include <iostream>

// Constructor - Initialize settings interface with current values
SettingsState::SettingsState(Game* game, StateMachine* machine)
    : m_game(game), m_machine(machine) {
    setupUI();
}

// State lifecycle - no special setup needed
void SettingsState::onEnter() {
}

// State lifecycle - save settings when exiting
void SettingsState::onExit() {
    // Persist audio settings to file
    SettingsManager::getInstance().saveAudioSettings(AudioManager::getInstance().getSettings());
    SettingsManager::getInstance().saveSettings();

    // Resume audio and reinitialize with new settings
    AudioManager::getInstance().resumeMusic();
    AudioManager::getInstance().initialize();
}

// Handle input events for buttons and sliders
void SettingsState::handleEvent(const sf::Event& event) {
    // Forward events to action buttons (Back/Apply)
    for (auto& button : m_actionButtons)
        button->handleEvent(event);

    // Forward events to volume sliders
    for (auto& slider : m_volumeSliders)
        slider->handleEvent(event);
}

// Update UI animations and slider interactions
void SettingsState::update(float dt) {
    // Update button animations
    for (auto& button : m_actionButtons)
        button->update(dt);

    // Update slider interactions and visual feedback
    for (auto& slider : m_volumeSliders)
        slider->update(dt);
}

// Render settings interface
void SettingsState::render(sf::RenderWindow& window) {
    window.draw(m_backgroundSprite);    // Background
    window.draw(m_titleText);           // "Settings" title

    // Render volume sliders with labels
    for (auto& slider : m_volumeSliders)
        slider->render(window);

    // Render action buttons
    for (auto& button : m_actionButtons)
        button->draw(window);
}

// Initialize complete settings UI layout
void SettingsState::setupUI() {
    auto& rm = ResourceManager::getInstance();
    sf::Font& font = rm.getFont("bruce");
    sf::Texture& bg = rm.getTexture("level_bg.png");
    sf::Texture& btnTex = rm.getTexture("btn.png");
    sf::Texture& knobTex = rm.getTexture("slider_knob.png");

    // Setup semi-transparent background
    m_backgroundSprite.setTexture(bg);
    m_backgroundSprite.setColor(sf::Color(255, 255, 255, 120));

    // Setup title text
    m_titleText.setFont(font);
    m_titleText.setCharacterSize(36);
    m_titleText.setString("Settings");
    m_titleText.setFillColor(sf::Color::White);

    // Center title horizontally
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setOrigin(titleBounds.width / 2.f, titleBounds.height / 2.f);
    m_titleText.setPosition(GameConstants::WINDOW_WIDTH / 2.f, 100.f);

    // Slider configuration
    const float sliderWidth = 300.f;
    const float sliderHeight = 10.f;
    sf::Vector2f sliderSize(sliderWidth, sliderHeight);
    const float sliderX = GameConstants::WINDOW_WIDTH / 2.f - sliderWidth / 2.f;
    const float sliderYStart = 180.f;
    const float sliderSpacing = 90.f;

    // Load current volume settings
    float master = SettingsManager::getInstance().getMasterVolume();
    float music = SettingsManager::getInstance().getMusicVolume();
    float sfx = SettingsManager::getInstance().getSfxVolume();

    // Helper function to create standardized sliders
    auto createSlider = [&](const std::string& label, float value, float yOffset) {
        auto slider = std::make_unique<Slider>(sf::Vector2f(sliderX, yOffset), sliderSize, value);
        slider->setTexture(knobTex);
        slider->setFont(font);
        slider->setKnobScale(0.2f);     // Small knob size
        slider->setLabel(label);
        return slider;
        };

    // Create volume control sliders
    m_volumeSliders.push_back(createSlider("Master Volume", master, sliderYStart));
    m_volumeSliders.push_back(createSlider("Music Volume", music, sliderYStart + sliderSpacing));
    m_volumeSliders.push_back(createSlider("SFX Volume", sfx, sliderYStart + 2 * sliderSpacing));

    // Setup action buttons (Back/Apply)
    m_actionButtons.clear();
    sf::Vector2f buttonSize(180.f, 60.f);
    float buttonY = sliderYStart + 3 * sliderSpacing + 30.f;
    float buttonSpacing = 40.f;

    // Center buttons horizontally
    float totalWidth = 2 * buttonSize.x + buttonSpacing;
    float startX = GameConstants::WINDOW_WIDTH / 2.f - totalWidth / 2.f;

    // Back button - return without saving
    auto backBtn = std::make_unique<Button>(
        sf::Vector2f(startX, buttonY), buttonSize, btnTex, sf::IntRect(), font, "Back");
    backBtn->setClickCallback([this]() { onBack(); });
    m_actionButtons.push_back(std::move(backBtn));

    // Apply button - save and apply settings
    auto applyBtn = std::make_unique<Button>(
        sf::Vector2f(startX + buttonSize.x + buttonSpacing, buttonY),
        buttonSize, btnTex, sf::IntRect(), font, "Apply");
    applyBtn->setClickCallback([this]() { onApply(); });
    m_actionButtons.push_back(std::move(applyBtn));
}

// Return to previous state without saving changes
void SettingsState::onBack() {
    if (m_machine)
        m_machine->popState();
}

// Apply current slider values to settings managers
void SettingsState::onApply() {
    // Read current slider values (0.0 to 1.0 range)
    float master = m_volumeSliders[0]->getValue();
    float music = m_volumeSliders[1]->getValue();
    float sfx = m_volumeSliders[2]->getValue();

    // Apply to SettingsManager (expects 0-100 scale)
    auto& sm = SettingsManager::getInstance();
    sm.setMasterVolume(master * 100.f);
    sm.setMusicVolume(music * 100.f);
    sm.setSfxVolume(sfx * 100.f);

    // Apply settings to AudioManager for immediate effect
    AudioManager::getInstance().applySettings(sm.getCurrentSettings().audio);
    AudioManager::getInstance().updateVolumes();

    // Save settings to persistent storage
    sm.saveSettings();

    // Play confirmation sound
    AudioManager::getInstance().playSound("click", AudioManager::AudioCategory::UI);
}