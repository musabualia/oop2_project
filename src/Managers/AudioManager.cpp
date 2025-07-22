// ================================
// Audio Manager - Singleton Pattern for Centralized Audio System Management
// ================================
#include "Managers/AudioManager.h"
#include "Managers/SettingsManager.h"
#include <iostream>
#include <algorithm>

// Singleton access point
AudioManager& AudioManager::getInstance() {
    static AudioManager instance;
    return instance;
}

// Initialize audio system and load all sound assets
void AudioManager::initialize() {
    // Load settings from persistent storage
    m_settings = SettingsManager::getInstance().getCurrentSettings().audio;
    updateVolumes();  // Apply volume settings

    // === Load Sound Effects (SFX) ===
    loadSound("button_hover", "button_hover.wav");
    loadSound("click", "click.wav");
    loadSound("coin_added", "coin_added.wav");
    loadSound("health_kit_added", "health_kit_added.wav");
    loadSound("explosion", "explosion.wav");
    loadSound("placed", "placed.wav");
    loadSound("killed", "killed.wav");
    loadSound("level_up", "level_up.wav");
    loadSound("gameover", "gameover.wav");
    loadSound("error", "error_action.wav");

    // Combat sounds
    loadSound("heavy_gunner_shot", "heavy_gunner_shot.wav");
    loadSound("robot_bullet", "robot_bullet.wav");
    loadSound("sniper_shot", "sniper_shot.wav");
    loadSound("shield_barier_fight", "shield_barier_fight.wav");

    // Hit and damage sounds
    loadSound("stealth_robot_hit", "stealth_robot_hit.wav");
    loadSound("rock_robot_hit", "rock_robot_hit.wav");

    // Death sounds
    loadSound("squad_died", "squad_died.wav");
    loadSound("rock_robot_die", "rock_robot_die.wav");
    loadSound("shield_died", "shield_died.wav");
    loadSound("stealth_robot_die", "stealth_robot_die.wav");

    // UI sounds
    loadSound("more", "more.wav");

    // === Load Background Music ===
    loadMusic("Menu", "menu_state.ogg");
    loadMusic("Level", "lvl_state.ogg");
    loadMusic("Menu1", "Menu1.ogg");
    loadMusic("game_bg", "game_bg.ogg");
}

// Cleanup and shutdown audio system
void AudioManager::shutdown() {
    stopAllSounds();
    stopMusic();
    m_soundBuffers.clear();
    m_musicTracks.clear();
    m_currentMusic = nullptr;
}

// Load sound buffer from file
bool AudioManager::loadSound(const std::string& name, const std::string& filename) {
    auto buffer = std::make_unique<sf::SoundBuffer>();
    if (!buffer->loadFromFile(getFullPath(filename))) {
        std::cerr << "Failed to load sound: " << filename << '\n';
        return false;
    }
    m_soundBuffers[name] = std::move(buffer);
    return true;
}

// Load music track from file
bool AudioManager::loadMusic(const std::string& name, const std::string& filename) {
    auto music = std::make_unique<sf::Music>();
    if (!music->openFromFile(getFullPath(filename))) {
        std::cerr << "Failed to load music: " << filename << '\n';
        return false;
    }
    m_musicTracks[name] = std::move(music);
    return true;
}

// Play sound with default SFX category
void AudioManager::playSound(const std::string& name, float volume) {
    playSound(name, AudioCategory::SFX, volume);
}

// Play sound with specific category and volume
void AudioManager::playSound(const std::string& name, AudioCategory category, float volume) {
    // Check mute and category enable states
    if (m_settings.muted) return;
    if ((category == AudioCategory::SFX && !m_settings.sfxEnabled) ||
        (category == AudioCategory::Music && !m_settings.musicEnabled)) return;

    // Find sound buffer
    auto it = m_soundBuffers.find(name);
    if (it == m_soundBuffers.end()) return;

    // Create and configure sound instance
    auto sound = std::make_unique<sf::Sound>();
    sound->setBuffer(*it->second);
    sound->setVolume(computeFinalVolume(category, volume));
    sound->play();

    // Cleanup old sounds periodically for memory management
    if (m_activeSounds.size() > 250) {
        cleanupStoppedSounds();
    }

    // Add to active sounds list
    m_activeSounds.push_back(std::move(sound));
}

// Stop all currently playing sounds
void AudioManager::stopAllSounds() {
    for (auto& sound : m_activeSounds)
        sound->stop();
    m_activeSounds.clear();
}

// Play background music with looping option
void AudioManager::playMusic(const std::string& name, bool loop) {
    if (m_settings.muted || !m_settings.musicEnabled) return;

    auto it = m_musicTracks.find(name);
    if (it == m_musicTracks.end()) return;

    // Stop current music if playing
    if (m_currentMusic && m_currentMusic->getStatus() == sf::Music::Playing)
        m_currentMusic->stop();

    // Start new music
    m_currentMusic = it->second.get();
    m_currentMusicName = name;
    m_currentMusic->setLoop(loop);
    m_currentMusic->setVolume(computeFinalVolume(AudioCategory::Music, 1.0f));
    m_currentMusic->play();
}

// Stop current background music
void AudioManager::stopMusic() {
    if (m_currentMusic)
        m_currentMusic->stop();
    m_currentMusic = nullptr;
}

// Pause current music
void AudioManager::pauseMusic() {
    if (m_currentMusic)
        m_currentMusic->pause();
}

// Resume paused music
void AudioManager::resumeMusic() {
    if (m_currentMusic && m_settings.musicEnabled)
        m_currentMusic->play();
}

// Set music looping state
void AudioManager::setMusicLoop(bool loop) {
    if (m_currentMusic)
        m_currentMusic->setLoop(loop);
}

// Update volume levels for all active audio
void AudioManager::updateVolumes() {
    // Calculate final volumes with master volume applied
    float master = m_settings.masterVolume / 100.0f;
    float music = m_settings.musicVolume / 100.0f * master;
    float sfx = m_settings.sfxVolume / 100.0f * master;
    float ui = m_settings.uiVolume / 100.0f * master;

    // Update current music volume
    if (m_currentMusic)
        m_currentMusic->setVolume(m_settings.musicEnabled ? music * 100.0f : 0.0f);

    // Update all active sound volumes
    for (auto& sound : m_activeSounds) {
        if (sound->getStatus() == sf::Sound::Playing) {
            sound->setVolume(m_settings.sfxEnabled ? sfx * 100.0f : 0.0f);
        }
    }
}

// Set master volume level
void AudioManager::setMasterVolume(float volume) {
    m_settings.masterVolume = volume;
}

// Set category-specific volume
void AudioManager::setCategoryVolume(AudioCategory category, float volume) {
    switch (category) {
    case AudioCategory::Music:
        m_settings.musicVolume = volume;
        break;
    case AudioCategory::SFX:
        m_settings.sfxVolume = volume;
        break;
    case AudioCategory::UI:
        m_settings.uiVolume = volume;
        break;
    }
}

// Get current volume for specific category
float AudioManager::getCategoryVolume(AudioCategory category) const {
    switch (category) {
    case AudioCategory::Music: return m_settings.musicVolume;
    case AudioCategory::SFX: return m_settings.sfxVolume;
    case AudioCategory::UI: return m_settings.uiVolume;
    default: return 100.f;
    }
}

// Set global mute state
void AudioManager::setMuted(bool muted) {
    m_settings.muted = muted;
    updateVolumes();
}

// Check if audio is muted
bool AudioManager::isMuted() const {
    return m_settings.muted;
}

// Per-frame update for cleanup
void AudioManager::update(float) {
    cleanupStoppedSounds();
}

// Apply complete audio settings
void AudioManager::applySettings(const AudioSettings& settings) {
    m_settings = settings;
    updateVolumes();
}

// Get current audio settings
AudioManager::AudioSettings AudioManager::getSettings() const {
    return m_settings;
}

// Private Helper Methods

// Remove stopped sounds from memory
void AudioManager::cleanupStoppedSounds() {
    m_activeSounds.erase(
        std::remove_if(m_activeSounds.begin(), m_activeSounds.end(),
            [](const std::unique_ptr<sf::Sound>& s) {
                return s->getStatus() == sf::Sound::Stopped;
            }),
        m_activeSounds.end());
}

// Calculate final volume considering category and master volume
float AudioManager::computeFinalVolume(AudioCategory category, float volume) const {
    if (m_settings.muted) return 0.0f;

    float catVolume = getCategoryVolume(category);
    return volume * (catVolume / 100.f) * (m_settings.masterVolume / 100.f);
}

// Get full file path for audio files
std::string AudioManager::getFullPath(const std::string& filename) const {
    return filename; // Files are in working directory
}