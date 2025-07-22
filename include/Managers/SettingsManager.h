#pragma once
#include "Core/Constants.h"
#include "Managers/AudioManager.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class SettingsManager {
public:
    // All game settings structure
    struct AllSettings {
        AudioManager::AudioSettings audio;
        bool fullscreen = false;
        bool vsync = true;
        int resolutionIndex = 0;
        std::vector<int> keyBindings;
        float gameSpeed = 1.0f;
        bool showTutorial = true;
        bool autoSave = true;
    };

    // Singleton access
    static SettingsManager& getInstance();

    void setResolution(const sf::Vector2u& resolution);
    sf::Vector2u getResolution() const;
    void setFullscreen(bool fullscreen);
    bool isFullscreen() const;
    void setVSync(bool vsync);
    bool isVSync() const;
    void applyGraphicsSettings(const sf::Vector2u& resolution, bool fullscreen, bool vsync);

    // Resolution utilities
    std::vector<sf::Vector2u> getSupportedResolutions() const;
    std::string getResolutionString(const sf::Vector2u& resolution) const;
    bool isResolutionSupported(const sf::Vector2u& resolution) const;

    // Settings change tracking
    bool hasSettingsChanged() const;
    void markSettingsApplied();

    bool saveAllSettings(const AllSettings& settings, const std::string& filename = "settings.cfg");
    bool loadAllSettings(AllSettings& settings, const std::string& filename = "settings.cfg");
    void applyAllSettings(const AllSettings& settings);
    void resetToDefaults(AllSettings& settings);

    // Individual category management
    bool saveAudioSettings(const AudioManager::AudioSettings& audioSettings);
    bool loadAudioSettings(AudioManager::AudioSettings& audioSettings);
    bool saveGraphicsSettings(bool fullscreen, bool vsync, int resolutionIndex);
    bool loadGraphicsSettings(bool& fullscreen, bool& vsync, int& resolutionIndex);
    bool saveControlsSettings(const std::vector<int>& keyBindings);
    bool loadControlsSettings(std::vector<int>& keyBindings);
    bool saveGameplaySettings(float gameSpeed, bool showTutorial, bool autoSave);
    bool loadGameplaySettings(float& gameSpeed, bool& showTutorial, bool& autoSave);

    // Default values
    AllSettings getDefaultSettings() const;
    AudioManager::AudioSettings getDefaultAudioSettings() const;

    // Current settings access

    const AllSettings& getCurrentSettings() const { return m_currentSettings; }
    // === Audio Getters ===
    float getMasterVolume() const;
    float getMusicVolume() const;
    float getSfxVolume() const;

    // === Audio Setters ===
    void setMasterVolume(float volume);
    void setMusicVolume(float volume);
    void setSfxVolume(float volume);

    // === Save shortcut ===
    void saveSettings();


    // Error handling
    std::string getLastError() const { return m_lastError; }
    bool hasErrors() const { return !m_lastError.empty(); }

private:
    SettingsManager() = default;
    ~SettingsManager() = default;
    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;

    // Current settings state
    AllSettings m_currentSettings;
    bool m_settingsChanged = false;
    mutable std::string m_lastError;

    // Helper methods
    bool writeToFile(const std::string& filename, const std::string& data);
    bool readFromFile(const std::string& filename, std::string& data);
    std::string serializeSettings(const AllSettings& settings);
    bool deserializeSettings(const std::string& data, AllSettings& settings);
    int findResolutionIndex(const sf::Vector2u& resolution) const;
    void setError(const std::string& error) const;
    void clearError() const;
};