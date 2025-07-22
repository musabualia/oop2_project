// ================================
// include/Systems/EventSystem.h 
// ================================
#pragma once
#include "Core/Constants.h"
#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <queue>
#include <type_traits>

class EventSystem {
public:
    // Singleton access
    static EventSystem& getInstance() {
        static EventSystem instance;
        return instance;
    }

    template<typename EventType>
    using EventHandler = std::function<void(const EventType&)>;

    template<typename EventType>
    void subscribe(EventHandler<EventType> handler) {
        static_assert(std::is_class_v<EventType>, "EventType must be a class");
        auto typeIndex = std::type_index(typeid(EventType));
        m_handlers[typeIndex].emplace_back([handler = std::move(handler)](const void* event) {
            handler(*static_cast<const EventType*>(event));
            });
    }

    // ADVANCED: Variadic templates + Perfect forwarding
    template<typename EventType, typename... Args>
    void publish(Args&&... args) {
        static_assert(std::is_class_v<EventType>, "EventType must be a class");
        auto eventCopy = std::make_unique<EventType>(std::forward<Args>(args)...);
        m_eventQueue.push({ std::type_index(typeid(EventType)), std::move(eventCopy) });
    }

    // Immediate event publishing
    template<typename EventType>
    void publishImmediate(const EventType& event) {
        static_assert(std::is_class_v<EventType>, "EventType must be a class");
        auto typeIndex = std::type_index(typeid(EventType));
        auto it = m_handlers.find(typeIndex);
        if (it != m_handlers.end()) {
            for (const auto& handler : it->second) {
                handler(&event);
            }
        }
    }

    // Process queued events
    void processEvents();

    // Clear all handlers and events
    void clear();

    // Statistics
    int getHandlerCount() const;
    int getQueuedEventCount() const;

private:
    EventSystem() = default;
    ~EventSystem() = default;
    EventSystem(const EventSystem&) = delete;
    EventSystem& operator=(const EventSystem&) = delete;

    // ADVANCED: Type-erased handler storage
    using TypeErasedHandler = std::function<void(const void*)>;
    std::unordered_map<std::type_index, std::vector<TypeErasedHandler>> m_handlers;

    // ADVANCED: Custom deleter for type-erased events
    struct QueuedEvent {
        std::type_index type;
        std::unique_ptr<void, void(*)(void*)> event;

        template<typename T>
        QueuedEvent(std::type_index t, std::unique_ptr<T> e)
            : type(t), event(e.release(), [](void* ptr) { delete static_cast<T*>(ptr); }) {
        }
    };
    std::queue<QueuedEvent> m_eventQueue;
};

struct RobotDestroyedEvent {
    class Robot* robot;
    sf::Vector2f position;
    int coinsRewarded;
    GameTypes::RobotType robotType;
};

struct SquadMemberPlacedEvent {
    class SquadMember* member;
    int lane;
    sf::Vector2f position;
    GameTypes::SquadMemberType memberType;
};

struct WaveStartedEvent {
    int waveNumber;
    int totalEnemies;
    float waveDifficulty;
};

struct ProjectileHitEvent {
    class Projectile* projectile;
    class Robot* target;
    int damage;
    sf::Vector2f hitPosition;
    bool wasCritical;
};

struct GameOverEvent {
    bool victory;
    int finalScore;
    int wavesCompleted;
    float timeElapsed;
};

struct BombExplosionEvent {
    sf::Vector2f position;
    float explosionRadius;
    int damage;

    BombExplosionEvent(const sf::Vector2f& pos, float radius, int dmg)
        : position(pos), explosionRadius(radius), damage(dmg) {
    }
};

// ✅ UPDATED: Drop Events with robot reward values
struct CoinDropEvent {
    sf::Vector2f position;
    int robotRewardValue;  // ✅ CHANGED: Use robot's actual reward value

    CoinDropEvent(const sf::Vector2f& pos, int rewardValue)
        : position(pos), robotRewardValue(rewardValue) {
    }
};

struct HealthPackDropEvent {
    sf::Vector2f position;
    int healAmount;

    HealthPackDropEvent(const sf::Vector2f& pos, int heal = 50)
        : position(pos), healAmount(heal) {
    }
};