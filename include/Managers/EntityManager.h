#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <type_traits>

template<typename EntityType>
class EntityManager {
public:
    using EntityPtr = std::unique_ptr<EntityType>;      // Pointer to entity
    using EntityContainer = std::vector<EntityPtr>;     // Container of entities
    using EntityRawPtr = EntityType*;                   // Raw pointer type

    EntityManager() = default;
    virtual ~EntityManager() = default;

    // Add entities
    void addEntity(EntityPtr entity) {
        if (entity) m_entities.push_back(std::move(entity));
    }

    // Create and add an entity of a derived type
    template<typename DerivedType, typename... Args>
    DerivedType* createEntity(Args&&... args) {
        static_assert(std::is_base_of_v<EntityType, DerivedType>, "DerivedType must inherit from EntityType");
        auto entity = std::make_unique<DerivedType>(std::forward<Args>(args)...);
        DerivedType* ptr = entity.get();
        addEntity(std::move(entity));
        return ptr;
    }

    // Core functionality
    virtual void update(float dt) {
        for (auto& entity : m_entities) {
            if (entity && entity->isActive()) entity->update(dt);
        }
    }

    virtual void draw(sf::RenderWindow& window) const {
        for (const auto& entity : m_entities) {
            if (entity && entity->isActive()) entity->draw(window);
        }
    }

    // Remove all inactive entities
    void removeInactive() {
        m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(),
            [](const EntityPtr& e) { return !e || !e->isActive(); }), m_entities.end());
    }

    // Remove a specific entity
    void removeEntity(EntityType* target) {
        m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(),
            [target](const EntityPtr& e) { return e.get() == target; }), m_entities.end());
    }

    // Find entities matching a condition
    template<typename Predicate>
    std::vector<EntityRawPtr> findEntities(Predicate pred) const {
        std::vector<EntityRawPtr> result;
        for (const auto& e : m_entities) {
            if (e && e->isActive() && pred(*e)) result.push_back(e.get());
        }
        return result;
    }

    // Find the first entity matching a condition
    template<typename Predicate>
    EntityRawPtr findFirstEntity(Predicate pred) const {
        auto it = std::find_if(m_entities.begin(), m_entities.end(),
            [&pred](const EntityPtr& e) { return e && e->isActive() && pred(*e); });
        return (it != m_entities.end()) ? it->get() : nullptr;
    }

    // Apply a function to each active entity
    template<typename Function>
    void forEachEntity(Function func) {
        for (auto& e : m_entities) {
            if (e && e->isActive()) func(*e);
        }
    }

    const EntityContainer& getEntities() const { return m_entities; }
    EntityContainer& getEntities() { return m_entities; }

    std::vector<EntityRawPtr> getEntityPointers() const {
        std::vector<EntityRawPtr> pointers;
        pointers.reserve(m_entities.size());
        for (const auto& e : m_entities) {
            if (e && e->isActive()) pointers.push_back(e.get());
        }
        return pointers;
    }

    void clear() { m_entities.clear(); }               // Clear all entities
    size_t size() const { return m_entities.size(); }  // Total entities
    size_t activeSize() const {                        // Number of active entities
        return std::count_if(m_entities.begin(), m_entities.end(),
            [](const EntityPtr& e) { return e && e->isActive(); });
    }
    bool empty() const { return m_entities.empty(); }  // Check if empty

protected:
    EntityContainer m_entities; // Storage for entities
};
