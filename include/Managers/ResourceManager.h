#pragma once
#include "Core/Constants.h"
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <memory>
#include <string>
#include <stdexcept>

class ResourceManager {
public:
    static ResourceManager& getInstance(); // Singleton access

    sf::Texture& getTexture(const std::string& filename);
    sf::Font& getFont(const std::string& filename);
    bool loadTexture(const std::string& name, const std::string& filename);
    bool loadFont(const std::string& name, const std::string& filename);
    bool hasTexture(const std::string& name) const;
   
    bool hasFont(const std::string& name) const;
    void unloadTexture(const std::string& name);
    void unloadFont(const std::string& name);
    void unloadAll();

    void loadTextureAtlas(const std::string& atlasName, const std::string& filename);

    void preloadGameAssets();
    void preloadMenuAssets();
    void preloadAnimationAssets();

    size_t getTextureCount() const;
    size_t getFontCount() const;

    size_t getTotalMemoryUsage() const;

private:
    ResourceManager() = default;
    ~ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::unordered_map<std::string, std::shared_ptr<sf::Texture>> m_textures; // Texture storage
    std::unordered_map<std::string, std::shared_ptr<sf::Font>> m_fonts;       // Font storage

    // Generic resource loading helper
    template<typename T>
    std::shared_ptr<T> loadResource(const std::string& filename);
};
