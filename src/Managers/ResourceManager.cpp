// ================================
// Resource Manager - Singleton Pattern for Centralized Asset Loading and Caching
// ================================
#include "Managers/ResourceManager.h"

// Singleton access point
ResourceManager& ResourceManager::getInstance() {
    static ResourceManager instance;
    return instance;
}

// Get texture with automatic loading if not cached
sf::Texture& ResourceManager::getTexture(const std::string& filename) {
    if (!hasTexture(filename)) {
        loadTexture(filename, filename);
    }
    return *m_textures[filename];
}

// Get font with automatic loading if not cached
sf::Font& ResourceManager::getFont(const std::string& filename) {
    if (!hasFont(filename)) {
        loadFont(filename, filename);
    }
    return *m_fonts[filename];
}

// Check if texture is already loaded
bool ResourceManager::hasTexture(const std::string& name) const {
    return m_textures.find(name) != m_textures.end();
}

// Check if font is already loaded
bool ResourceManager::hasFont(const std::string& name) const {
    return m_fonts.find(name) != m_fonts.end();
}

// Unload specific texture to free memory
void ResourceManager::unloadTexture(const std::string& name) {
    m_textures.erase(name);
}

// Unload specific font to free memory
void ResourceManager::unloadFont(const std::string& name) {
    m_fonts.erase(name);
}

// Unload all resources for cleanup
void ResourceManager::unloadAll() {
    m_textures.clear();
    m_fonts.clear();
}

// Load texture atlas for sprite sheets
void ResourceManager::loadTextureAtlas(const std::string& atlasName, const std::string& filename) {
    loadTexture(atlasName, filename);
}

// Preload all menu-related assets for smooth UI
void ResourceManager::preloadMenuAssets() {
    // Background textures
    loadTexture("menu_background.png", "menu_background.png");
    loadTexture("level_btn.png", "level_btn.png");

    // Level selection backgrounds (stitched together)
    loadTexture("levelbackground_left.png", "levelbackground_left.png");
    loadTexture("levelbackground_middle.png", "levelbackground_middle.png");
    loadTexture("levelbackground_right.png", "levelbackground_right.png");

    // Unit selection icons
    loadTexture("select1.png", "select1.png");           // HeavyGunner
    loadTexture("select2.png", "select2.png");           // Sniper
    loadTexture("select3.png", "select3.png");           // ShieldBearer
    loadTexture("select_medkit.png", "select_medkit.png"); // HealthPack
    loadTexture("select-b.png", "select-b.png");         // Bomb

    // UI elements
    loadTexture("slider_knob.png", "slider_knob.png");
    loadTexture("HealthPack.png", "HealthPack.png");
    loadTexture("Sniper_locked.png", "Sniper_locked.png");
    loadTexture("back_to_menu.png", "back_to_menu.png");
    loadTexture("select_level.png", "select_level.png");

    // Command icons
    loadTexture("undo.png", "undo.png");
    loadTexture("redo.png", "redo.png");
}

// Preload all gameplay assets
void ResourceManager::preloadGameAssets() {
    // Level background and grid
    loadTexture("level_bg.png", "level_bg.png");
    loadTexture("placing_slot.png", "placing_slot.png");

    // Projectile textures
    loadTexture("bullet.png", "bullet.png");
    loadTexture("SniperBullet.png", "SniperBullet.png");
    loadTexture("robot_bullet.png", "robot_bullet.png");

    // Game objects
    loadTexture("bomb.png", "bomb.png");
    loadTexture("coin.png", "coin.png");
    loadTexture("HealthBag.png", "HealthBag.png");

    // Load primary game font
    loadFont("bruce", "BruceForeverRegular-X3jd2.ttf");

    // Load animation assets
    preloadAnimationAssets();
}

// Preload all animation sprite sheets
void ResourceManager::preloadAnimationAssets() {
    // Effect animations
    loadTexture("explosion.png", "explosion.png");

    // Squad member animations
    // Heavy Gunner
    loadTexture("HeavyGunner_Idle.png", "HeavyGunner_Idle.png");
    loadTexture("HeavyGunner_Shot.png", "HeavyGunner_Shot.png");
    loadTexture("HeavyGunner_Dead.png", "HeavyGunner_Dead.png");

    // Sniper
    loadTexture("ss_Idle.png", "ss_Idle.png");
    loadTexture("Sniper_Shot.png", "Sniper_Shot.png");
    loadTexture("Sniper_Dead.png", "Sniper_Dead.png");

    // Shield Bearer
    loadTexture("ShieldBearer_Idle.png", "ShieldBearer_Idle.png");
    loadTexture("ShieldBearer_Block.png", "ShieldBearer_Block.png");
    loadTexture("ShieldBearer_Dead.png", "ShieldBearer_Dead.png");

    // Robot animations
    // Stealth Robot
    loadTexture("StealthRobot_Walk1.png", "StealthRobot_Walk1.png");
    loadTexture("StealthRobot_Hit.png", "StealthRobot_Hit.png");
    loadTexture("StealthRobot_Dead.png", "StealthRobot_Dead.png");

    // Fire Robot
    loadTexture("fire_robot_walk.png", "fire_robot_walk.png");
    loadTexture("fire_robot_death.png", "fire_robot_death.png");

    // Basic Robot (Rock Robot)
    loadTexture("RockRobot_Walk.png", "RockRobot_Walk.png");
    loadTexture("RobotRock_Dead.png", "RobotRock_Dead.png");
    loadTexture("RockRobot_Hit.png", "RockRobot_Hit.png");
}

// Private Helper Methods

// Load texture from file with error handling
bool ResourceManager::loadTexture(const std::string& name, const std::string& filename) {
    auto texture = std::make_shared<sf::Texture>();
    if (texture->loadFromFile(filename)) {
        m_textures[name] = texture;
        return true;
    }
    return false;
}

// Load font from file with fallback options
bool ResourceManager::loadFont(const std::string& name, const std::string& filename) {
    auto font = std::make_shared<sf::Font>();

    // Try multiple font paths for compatibility
    std::vector<std::string> fontPaths = {
        filename,                          // Game font
        "C:/Windows/Fonts/arial.ttf",     // Windows fallback
        "C:/Windows/Fonts/Arial.ttf"      // Windows fallback (case sensitive)
    };

    for (const auto& path : fontPaths) {
        if (font->loadFromFile(path)) {
            m_fonts[name] = font;
            return true;
        }
    }
    return false;
}

// Resource Statistics Methods

// Get number of loaded textures
size_t ResourceManager::getTextureCount() const {
    return m_textures.size();
}

// Get number of loaded fonts
size_t ResourceManager::getFontCount() const {
    return m_fonts.size();
}

// Estimate total memory usage (simplified calculation)
size_t ResourceManager::getTotalMemoryUsage() const {
    return (m_textures.size() + m_fonts.size()) * 1024; // Rough estimate
}

// Template helper for generic resource loading (for future extension)
template<typename T>
std::shared_ptr<T> ResourceManager::loadResource(const std::string& filename) {
    auto resource = std::make_shared<T>();
    if (resource->loadFromFile(filename)) {
        return resource;
    }
    return nullptr;
}