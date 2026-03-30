#ifndef FLUXBORN_KEYBOARD_EVENTS_H
#define FLUXBORN_KEYBOARD_EVENTS_H

#include "Event.h"
#include <sstream>

/**
 * @class KeyEvent
 * @brief Базовый класс для всех событий клавиатуры.
 * Содержит общие данные: код клавиши и модификаторы.
 */
class KeyEvent : public EventBase {
public:
    /** @return Код клавиши (обычно соответствует кодам GLFW). */
    inline int getKeyCode() const { return m_KeyCode; }

    /** @return Битовая маска модификаторов (Shift, Ctrl, Alt, Super). */
    inline int getModifiers() const { return m_Modifiers; }

    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

protected:
    /**
     * @brief Защищенный конструктор, так как KeyEvent — это абстракция.
     * @param keycode Код клавиши.
     * @param mods Модификаторы.
     */
    KeyEvent(int keycode, int mods)
    : m_KeyCode(keycode), m_Modifiers(mods) {}

    int m_KeyCode;
    int m_Modifiers;
};

/**
 * @class KeyPressedEvent
 * @brief Событие нажатия клавиши.
 */
class KeyPressedEvent : public KeyEvent {
public:
    /**
     * @param keycode Код нажатой клавиши.
     * @param mods Состояние модификаторов.
     * @param repeat Флаг удержания клавиши (true, если это автоповтор ОС).
     */
    KeyPressedEvent(int keycode, int mods, bool repeat = false)
    : KeyEvent(keycode, mods), m_IsRepeat(repeat) {}

    /** @return true, если клавиша удерживается. */
    inline bool isRepeat() const { return m_IsRepeat; }

    std::string toString() const override {
        std::stringstream ss;
        ss << "KeyPressedEvent: " << m_KeyCode << " (repeat: " << m_IsRepeat << ")";
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyPressed)
private:
    bool m_IsRepeat;
};

/**
 * @class KeyReleasedEvent
 * @brief Событие отпускания клавиши.
 */
class KeyReleasedEvent : public KeyEvent {
public:
    KeyReleasedEvent(int keycode, int mods)
    : KeyEvent(keycode, mods) {}

    std::string toString() const override {
        std::stringstream ss;
        ss << "KeyReleasedEvent: " << m_KeyCode;
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyReleased)
};

/**
 * @class KeyTypedEvent
 * @brief Событие ввода текстового символа.
 * В отличие от KeyPressed, это событие передает обработанный Unicode символ.
 * Полезно для реализации текстовых полей и чата.
 */
class KeyTypedEvent : public EventBase {
public:
    /** @param keycode Unicode код введенного символа. */
    KeyTypedEvent(unsigned int keycode)
    : m_KeyCode(keycode) {}

    /** @return Unicode код символа. */
    inline unsigned int getKeyCode() const { return m_KeyCode; }

    std::string toString() const override {
        std::stringstream ss;
        ss << "KeyTypedEvent: " << (char)m_KeyCode;
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyTyped)
    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
private:
    unsigned int m_KeyCode;
};

#endif // FLUXBORN_KEYBOARD_EVENTS_H
