#ifndef FLUXBORN_MOUSE_EVENTS_H
#define FLUXBORN_MOUSE_EVENTS_H

#include "Event.h"


/**
 * @class MouseButtonPressedEvent
 * @brief Событие нажатия кнопки мыши.
 */
class MouseButtonPressedEvent : public EventBase {
public:
    MouseButtonPressedEvent(int button, float x, float y)
    : m_Button(button), m_X(x), m_Y(y) {}

    inline int getMouseButton() const { return m_Button; }
    inline float getX() const { return m_X; }
    inline float getY() const { return m_Y; }

    EVENT_CLASS_TYPE(MouseButtonPressed)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
private:
    int m_Button;
    float m_X, m_Y;
};

/**
 * @class MouseButtonReleasedEvent
 * @brief Событие отпускания кнопки мыши.
 */
class MouseButtonReleasedEvent : public EventBase {
public:
    MouseButtonReleasedEvent(int button, float x, float y)
    : m_Button(button), m_X(x), m_Y(y) {}

    inline int getMouseButton() const { return m_Button; }

    EVENT_CLASS_TYPE(MouseButtonReleased)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
private:
    int m_Button;
    float m_X, m_Y;
};



/**
 * @class MouseMovedEvent
 * @brief Событие изменения координат курсора мыши.
 */
class MouseMovedEvent : public EventBase {
public:
    /**
     * @param x Текущая координата X.
     * @param y Текущая координата Y.
     */
    MouseMovedEvent(float x, float y) : m_MouseX(x), m_MouseY(y) {}

    /** @return Координата X курсора в пикселях окна. */
    inline float getX() const { return m_MouseX; }

    /** @return Координата Y курсора в пикселях окна. */
    inline float getY() const { return m_MouseY; }

    /** @brief Текстовое описание события для логов. */
    std::string toString() const override {
        return "MouseMoved: " + std::to_string(m_MouseX) + ", " + std::to_string(m_MouseY);
    }

    EVENT_CLASS_TYPE(MouseMoved)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

private:
    float m_MouseX, m_MouseY;
};



/**
 * @class MouseScrolledEvent
 * @brief Событие прокрутки колесика мыши.
 */
class MouseScrolledEvent : public EventBase {
public:
    MouseScrolledEvent(float xOffset, float yOffset)
    : m_XOffset(xOffset), m_YOffset(yOffset) {}

    inline float getXOffset() const { return m_XOffset; }
    inline float getYOffset() const { return m_YOffset; }

    std::string toString() const override {
        return "MouseScrolled: " + std::to_string(m_XOffset) + ", " + std::to_string(m_YOffset);
    }

    EVENT_CLASS_TYPE(MouseScrolled)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
private:
    float m_XOffset, m_YOffset;
};
#endif
