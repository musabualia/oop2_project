// ================================
// src/Systems/EventSystem.cpp - Implementation for your EventSystem
// ================================
#include "Systems/EventSystem.h"
#include <iostream>

void EventSystem::processEvents() {
    while (!m_eventQueue.empty()) {
        auto& queuedEvent = m_eventQueue.front();

        // Find handlers for this event type
        auto it = m_handlers.find(queuedEvent.type);
        if (it != m_handlers.end()) {
            // Call all handlers for this event type
            for (const auto& handler : it->second) {
                handler(queuedEvent.event.get());
            }
        }

        m_eventQueue.pop();
    }
}

void EventSystem::clear() {
    m_handlers.clear();

    // Clear the queue
    while (!m_eventQueue.empty()) {
        m_eventQueue.pop();
    }
}

int EventSystem::getHandlerCount() const {
    int total = 0;
    for (const auto& pair : m_handlers) {
        total += static_cast<int>(pair.second.size());
    }
    return total;
}

int EventSystem::getQueuedEventCount() const {
    return static_cast<int>(m_eventQueue.size());
}