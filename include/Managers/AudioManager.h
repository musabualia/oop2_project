#pragma once

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

class AudioManager {
public:
    // Types of audio categories
    enum class AudioCategory {
        Music,
        SFX,
        UI,
    };

    // Struct to hold audio volume and toggle settings
    struct AudioSettings {
        float masterVolume = 100.f;
        float musicVolume = 100.f;
        float sfxVolume = 100.f;
        float uiVolume = 100.f;
        bool muted = false;
        bool musicEnabled = true;
        bool sfxEnabled = true;
    };

    static AudioManager& getInstance(); // Singleton access

    void initialize(); // Initialize audio system
    void shutdown();   // Shutdown and cleanup

    bool loadSound(const std::string& name, const std::string& filename); // Load sound buffer
    bool loadMusic(const std::string& name, const std::string& filename); // Load music track

    void playSound(const std::string& name, float volume = 1.0f); // Play sound by name
    void playSound(const std::string& name, AudioCategory category, float volume = 1.0f); // Play sound with category
    void stopAllSounds(); // Stop all currently playing sounds

    void playMusic(const std::string& name, bool loop = true); // Play music track
    void stopMusic();       // Stop current music
    void pauseMusic();      // Pause music playback
    void resumeMusic();     // Resume paused music
    void setMusicLoop(bool loop); // Toggle music looping
    void updateVolumes();   // Apply volume settings

    void setMasterVolume(float volume); // Set master volume
    void setCategoryVolume(AudioCategory category, float volume); // Set specific category volume
    float getCategoryVolume(AudioCategory category) const; // Get current volume for category

    void setMuted(bool muted); // Mute or unmute all audio
    bool isMuted() const;      // Check if audio is muted

    void update(float dt); // Update active sounds per frame

    void applySettings(const AudioSettings& settings); // Apply complete settings
    AudioSettings getSettings() const; // Retrieve current settings

private:
    AudioManager() = default;
    ~AudioManager() = default;
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> m_soundBuffers; // Loaded sound buffers
    std::unordered_map<std::string, std::unique_ptr<sf::Music>> m_musicTracks;        // Loaded music tracks
    std::vector<std::unique_ptr<sf::Sound>> m_activeSounds;                           // Active playing sounds

    sf::Music* m_currentMusic = nullptr; // Pointer to current music
    std::string m_currentMusicName;      // Name of the current music
    AudioSettings m_settings;            // Current audio settings

    void cleanupStoppedSounds(); // Remove stopped sounds from memory
    float computeFinalVolume(AudioCategory category, float volume) const; // Calculate final volume level
    std::string getFullPath(const std::string& filename) const; // Resolve full file path
};
