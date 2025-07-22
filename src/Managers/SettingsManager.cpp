// ================================
// Settings Manager - Singleton Pattern for Persistent Game Configuration
// ================================
#include "Managers/SettingsManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

// Singleton access point
SettingsManager& SettingsManager::getInstance() {
    static SettingsManager instance;
    return instance;
}

// Graphics Settings Management
void SettingsManager::setResolution(const sf::Vector2u& resolution) {
    m_currentSettings.resolutionIndex = findResolutionIndex(resolution);
    m_settingsChanged = true;
}

sf::Vector2u SettingsManager::getResolution() const {
    auto resolutions = getSupportedResolutions();
    if (m_currentSettings.resolutionIndex >= 0 &&
        m_currentSettings.resolutionIndex < static_cast<int>(resolutions.size())) {
        return resolutions[m_currentSettings.resolutionIndex];
    }
    return sf::Vector2u(1200, 800); // Default resolution
}

void SettingsManager::setFullscreen(bool fullscreen) {
    m_currentSettings.fullscreen = fullscreen;
    m_settingsChanged = true;
}

bool SettingsManager::isFullscreen() const {
    return m_currentSettings.fullscreen;
}

void SettingsManager::setVSync(bool vsync) {
    m_currentSettings.vsync = vsync;
    m_settingsChanged = true;
}

bool SettingsManager::isVSync() const {
    return m_currentSettings.vsync;
}

// Apply all graphics settings at once
void SettingsManager::applyGraphicsSettings(const sf::Vector2u& resolution, bool fullscreen, bool vsync) {
    m_currentSettings.resolutionIndex = findResolutionIndex(resolution);
    m_currentSettings.fullscreen = fullscreen;
    m_currentSettings.vsync = vsync;
    m_settingsChanged = true;
}

// Get all supported screen resolutions
std::vector<sf::Vector2u> SettingsManager::getSupportedResolutions() const {
    std::vector<sf::Vector2u> resolutions;
    auto modes = sf::VideoMode::getFullscreenModes();

    // Filter to reasonable resolutions
    for (const auto& mode : modes) {
        if (mode.width >= 800 && mode.height >= 600)
            resolutions.emplace_back(mode.width, mode.height);
    }

    // Add common resolutions if missing
    std::vector<sf::Vector2u> common = {
        {800, 600}, {1024, 768}, {1200, 800}, {1280, 720},
        {1366, 768}, {1440, 900}, {1600, 900}, {1920, 1080}
    };
    for (const auto& res : common) {
        if (std::find(resolutions.begin(), resolutions.end(), res) == resolutions.end())
            resolutions.push_back(res);
    }

    // Sort by total pixel count
    std::sort(resolutions.begin(), resolutions.end(),
        [](const sf::Vector2u& a, const sf::Vector2u& b) {
            return a.x * a.y < b.x * b.y;
        });

    return resolutions;
}

// Track if settings have been modified
bool SettingsManager::hasSettingsChanged() const {
    return m_settingsChanged;
}

void SettingsManager::markSettingsApplied() {
    m_settingsChanged = false;
}

// Complete settings management
void SettingsManager::applyAllSettings(const AllSettings& settings) {
    m_currentSettings = settings;
    AudioManager::getInstance().applySettings(settings.audio);
    m_settingsChanged = true;
}

// Create default settings configuration
SettingsManager::AllSettings SettingsManager::getDefaultSettings() const {
    AllSettings defaults;
    defaults.audio = AudioManager::getInstance().getSettings();
    defaults.fullscreen = false;
    defaults.vsync = true;
    defaults.resolutionIndex = findResolutionIndex({ 1200, 800 });
    defaults.keyBindings = {
        sf::Keyboard::W, sf::Keyboard::S, sf::Keyboard::A,
        sf::Keyboard::D, sf::Keyboard::Space, sf::Keyboard::Escape
    };
    defaults.gameSpeed = 1.0f;
    defaults.showTutorial = true;
    defaults.autoSave = true;
    return defaults;
}

// Save all settings to file
bool SettingsManager::saveAllSettings(const AllSettings& settings, const std::string& filename) {
    m_currentSettings = settings;
    std::string data = serializeSettings(settings);
    return writeToFile(filename, data);
}

// Load settings from file with fallback to defaults
bool SettingsManager::loadAllSettings(AllSettings& settings, const std::string& filename) {
    std::string data;
    if (!readFromFile(filename, data)) {
        settings = getDefaultSettings();
        m_currentSettings = settings;
        return true; // Return true for defaults
    }
    if (!deserializeSettings(data, settings)) {
        settings = getDefaultSettings();
        m_currentSettings = settings;
        return false;
    }
    m_currentSettings = settings;
    return true;
}

// Audio settings management
bool SettingsManager::saveAudioSettings(const AudioManager::AudioSettings& audioSettings) {
    m_currentSettings.audio = audioSettings;
    return saveAllSettings(m_currentSettings);
}

void SettingsManager::resetToDefaults(AllSettings& settings) {
    settings = getDefaultSettings();
    m_currentSettings = settings;
    m_settingsChanged = true;
}

// Audio volume getters
float SettingsManager::getMasterVolume() const {
    return m_currentSettings.audio.masterVolume;
}

float SettingsManager::getMusicVolume() const {
    return m_currentSettings.audio.musicVolume;
}

float SettingsManager::getSfxVolume() const {
    return m_currentSettings.audio.sfxVolume;
}

// Audio volume setters with clamping
void SettingsManager::setMasterVolume(float volume) {
    m_currentSettings.audio.masterVolume = std::clamp(volume, 0.0f, 100.0f);
    m_settingsChanged = true;
}

void SettingsManager::setMusicVolume(float volume) {
    m_currentSettings.audio.musicVolume = std::clamp(volume, 0.0f, 100.0f);
    m_settingsChanged = true;
}

void SettingsManager::setSfxVolume(float volume) {
    m_currentSettings.audio.sfxVolume = std::clamp(volume, 0.0f, 100.0f);
    m_settingsChanged = true;
}

// Quick save current settings
void SettingsManager::saveSettings() {
    saveAllSettings(m_currentSettings);
}

// Helper: Find index of resolution in supported list
int SettingsManager::findResolutionIndex(const sf::Vector2u& resolution) const {
    auto resolutions = getSupportedResolutions();
    auto it = std::find(resolutions.begin(), resolutions.end(), resolution);
    return (it != resolutions.end()) ? static_cast<int>(std::distance(resolutions.begin(), it)) : 0;
}

// File I/O operations
bool SettingsManager::writeToFile(const std::string& filename, const std::string& data) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    file << data;
    return file.good();
}

bool SettingsManager::readFromFile(const std::string& filename, std::string& data) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    std::ostringstream buffer;
    buffer << file.rdbuf();
    data = buffer.str();
    return true;
}

// Serialize settings to INI format
std::string SettingsManager::serializeSettings(const AllSettings& settings) {
    std::ostringstream oss;

    // Audio section
    oss << "[Audio]\n";
    oss << "masterVolume=" << settings.audio.masterVolume << "\n";
    oss << "musicVolume=" << settings.audio.musicVolume << "\n";
    oss << "sfxVolume=" << settings.audio.sfxVolume << "\n";
    oss << "muted=" << (settings.audio.muted ? "true" : "false") << "\n";
    oss << "musicEnabled=" << (settings.audio.musicEnabled ? "true" : "false") << "\n";
    oss << "sfxEnabled=" << (settings.audio.sfxEnabled ? "true" : "false") << "\n\n";

    // Graphics section
    oss << "[Graphics]\n";
    oss << "fullscreen=" << (settings.fullscreen ? "true" : "false") << "\n";
    oss << "vsync=" << (settings.vsync ? "true" : "false") << "\n";
    oss << "resolutionIndex=" << settings.resolutionIndex << "\n\n";

    // Controls section
    oss << "[Controls]\n";
    for (size_t i = 0; i < settings.keyBindings.size(); ++i)
        oss << "key" << i << "=" << settings.keyBindings[i] << "\n";

    // Gameplay section
    oss << "\n[Gameplay]\n";
    oss << "gameSpeed=" << settings.gameSpeed << "\n";
    oss << "showTutorial=" << (settings.showTutorial ? "true" : "false") << "\n";
    oss << "autoSave=" << (settings.autoSave ? "true" : "false") << "\n";

    return oss.str();
}

// Deserialize settings from INI format
bool SettingsManager::deserializeSettings(const std::string& data, AllSettings& settings) {
    settings = getDefaultSettings(); // Start with defaults
    std::istringstream iss(data);
    std::string line, section;

    // Parse INI file line by line
    while (std::getline(iss, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;

        // Parse section headers
        if (line[0] == '[' && line.back() == ']') {
            section = line.substr(1, line.length() - 2);
            continue;
        }

        // Parse key=value pairs
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string value = line.substr(eq + 1);

        // Apply values based on section
        if (section == "Audio") {
            if (key == "masterVolume") settings.audio.masterVolume = std::stof(value);
            else if (key == "musicVolume") settings.audio.musicVolume = std::stof(value);
            else if (key == "sfxVolume") settings.audio.sfxVolume = std::stof(value);
            else if (key == "muted") settings.audio.muted = (value == "true");
            else if (key == "musicEnabled") settings.audio.musicEnabled = (value == "true");
            else if (key == "sfxEnabled") settings.audio.sfxEnabled = (value == "true");
        }
        else if (section == "Graphics") {
            if (key == "fullscreen") settings.fullscreen = (value == "true");
            else if (key == "vsync") settings.vsync = (value == "true");
            else if (key == "resolutionIndex") settings.resolutionIndex = std::stoi(value);
        }
        else if (section == "Controls" && key.substr(0, 3) == "key") {
            size_t index = std::stoul(key.substr(3));
            if (index < settings.keyBindings.size())
                settings.keyBindings[index] = std::stoi(value);
        }
        else if (section == "Gameplay") {
            if (key == "gameSpeed") settings.gameSpeed = std::stof(value);
            else if (key == "showTutorial") settings.showTutorial = (value == "true");
            else if (key == "autoSave") settings.autoSave = (value == "true");
        }
    }
    return true;
}