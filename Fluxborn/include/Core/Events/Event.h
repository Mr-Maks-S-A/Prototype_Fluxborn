#ifndef FLUXBORN_EVENT_H
#define FLUXBORN_EVENT_H

#include <string>

/**
 * @defgroup Events Система Событий
 * @brief Группа классов для управления сигналами и вводом в движке Fluxborn.
 * @{
 */

/**
 * @enum EventType
 * @brief Перечисление всех уникальных типов событий.
 */
enum class EventType {
    None = 0,
    WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
    AppTick, AppUpdate, AppRender,
    KeyPressed, KeyReleased, KeyTyped,
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
};

/**
 * @enum EventCategory
 * @brief Битовые маски для фильтрации событий по категориям.
 * Позволяет обрабатывать, например, все события мыши одним фильтром.
 */
enum EventCategory {
    None = 0,
    EventCategoryApplication    = 1 << 0, ///< События жизненного цикла приложения
    EventCategoryInput          = 1 << 1, ///< События любого пользовательского ввода
    EventCategoryKeyboard       = 1 << 2, ///< Клавиатурные события
    EventCategoryMouse          = 1 << 3, ///< Движение мыши и прокрутка
    EventCategoryMouseButton    = 1 << 4  ///< Нажатия кнопок мыши
};

/**
 * @class EventBase
 * @brief Абстрактный интерфейс для всех событий во Fluxborn.
 * * Базовый класс предоставляет механизмы для определения типа события
 * и управления его состоянием (обработано/не обработано).
 */
class EventBase {
public:
    virtual ~EventBase() = default;

    /**
     * @brief Флаг остановки события.
     * Если установлен в true, последующие подписчики не получат это событие.
     */
    bool Handled = false;

    /** @brief Возвращает динамический тип события. */
    virtual EventType getEventType() const = 0;

    /** @brief Возвращает имя события (для отладки). */
    virtual const char* getName() const = 0;

    /** @brief Возвращает битовую маску категорий события. */
    virtual int getCategoryFlags() const = 0;

    /** @brief Форматирует данные события в строку. */
    virtual std::string toString() const { return getName(); }

    /**
     * @brief Проверяет принадлежность события к заданной категории.
     * @param category Категория для проверки.
     * @return true если событие входит в категорию.
     */
    bool isInCategory(EventCategory category) {
        return getCategoryFlags() & (int)category;
    }
};

/**
 * @def EVENT_CLASS_TYPE(type)
 * @brief Вспомогательный макрос для реализации методов типа в наследниках.
 * Реализует статическую проверку типа для шаблонов и динамическую для интерфейса.
 */
#define EVENT_CLASS_TYPE(type) \
static EventType getStaticType() { return EventType::type; } \
virtual EventType getEventType() const override { return getStaticType(); } \
virtual const char* getName() const override { return #type; }

/**
 * @def EVENT_CLASS_CATEGORY(category)
 * @brief Вспомогательный макрос для назначения категорий событию.
 */
#define EVENT_CLASS_CATEGORY(category) \
virtual int getCategoryFlags() const override { return category; }

/** @} */ // Конец группы Events
#endif
