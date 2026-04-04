#ifndef FLUXBORN_EVENT_MANAGER_H
#define FLUXBORN_EVENT_MANAGER_H

#include "Event.h"
#include <vector>
#include <map>
#include <queue>
#include <memory>
#include <mutex>
#include <functional>

/**
 * @class EventManager
 * @brief Диспетчер событий, реализующий паттерн "Издатель-Подписчик".
 * * Менеджер поддерживает два режима работы:
 * 1. Мгновенная рассылка (emit) — блокирует поток до завершения обработки.
 * 2. Очередь событий (enqueue) — потокобезопасно сохраняет событие для обработки в конце кадра.
 * * @note Класс является синглтоном.
 */
class EventManager {
public:
    /** @brief Тип функции-обратного вызова для обработки базового события. */
    using EventCallback = std::function<void(EventBase&)>;

    /** @brief Возвращает единственный экземпляр менеджера. */
    static EventManager& getInstance() {
        static EventManager instance;
        return instance;
    }

    /**
     * @brief Регистрирует подписчика на определенный тип события.
     * * Использование:
     * @code
     * EventManager::getInstance().subscribe<MouseMovedEvent>([](MouseMovedEvent& e) { ... });
     * @endcode
     * * @tparam T Тип события, на которое оформляется подписка.
     * @param callback Лямбда или функция-обработчик.
     */
    template<typename T>
    void subscribe(std::function<void(T&)> callback) {
        m_subscribers[T::getStaticType()].push_back([callback](EventBase& e) {
            callback(static_cast<T&>(e));
        });
    }

    /**
     * @brief Мгновенно рассылает событие всем подписчикам.
     * * Событие создается на стеке и уничтожается сразу после вызова всех обработчиков.
     * @tparam T Тип создаваемого события.
     * @tparam Args Типы аргументов конструктора события.
     * @param args Аргументы для конструктора события T.
     */
    template<typename T, typename... Args>
    void emit(Args&&... args) {
        T e(std::forward<Args>(args)...);
        dispatch(e);
    }

    /**
     * @brief Помещает событие в защищенную мьютексом очередь.
     * * Безопасно для вызова из других потоков (например, сетевого или физического).
     * Событие будет обработано только при вызове processQueue().
     */
    template<typename T, typename... Args>
    void enqueue(Args&&... args) {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_eventQueue.push(std::make_unique<T>(std::forward<Args>(args)...));
    }

    /**
     * @brief Разбирает накопленную очередь событий.
     * Должен вызываться один раз за итерацию игрового цикла.
     */
    void processQueue();

private:
    EventManager() = default;
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    /** @brief Внутренний метод распределения события по подписчикам. */
    void dispatch(EventBase& e);

    std::map<EventType, std::vector<EventCallback>> m_subscribers;
    std::queue<std::unique_ptr<EventBase>> m_eventQueue;
    std::mutex m_queueMutex;
};

#endif
