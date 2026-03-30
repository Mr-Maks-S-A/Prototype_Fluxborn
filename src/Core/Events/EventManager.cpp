#include "Core/Events/EventManager.h"


void EventManager::dispatch(EventBase& e) {
    auto type = e.getEventType();
    // Исправлено: m_subscribers вместо m_Subscribers
    if (m_subscribers.find(type) != m_subscribers.end()) {
        for (auto& callback : m_subscribers[type]) {
            callback(e);
        }
    }
}

void EventManager::processQueue() {
    // Добавляем lock, так как очередь защищена мьютексом (как указано в .h)
    std::lock_guard<std::mutex> lock(m_queueMutex);

    // Исправлено: m_eventQueue вместо m_EventQueue
    while (!m_eventQueue.empty()) {
        auto& e = m_eventQueue.front();
        dispatch(*e);
        m_eventQueue.pop();
    }
}
