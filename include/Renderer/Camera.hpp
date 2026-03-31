/**
 * @file Camera.hpp
 * @brief Система навигации в 3D пространстве (FPS-стиль).
 */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>


/**
 * @class Camera
 * @brief Объект камеры, вычисляющий матрицы вида и проекции.
 * * Поддерживает:
 * - Свободное перемещение (WASD + Space/Ctrl).
 * - Вращение мышью с ограничением тангажа (Pitch).
 * - Динамическое изменение FOV (зум).
 * - Систему подписки на события через EventManager.
 */
class Camera {
public:
    /**
     * @brief Конструктор камеры.
     * @param position Стартовая позиция в мире.
     * @param up Вектор "верха" мира (обычно {0, 1, 0}).
     * @param yaw Угол поворота вокруг вертикальной оси (рыскание).
     * @param pitch Угол наклона вверх/вниз (тангаж).
     */
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f, float pitch = 0.0f);

    /** @name Матрицы трансформации */
    ///@{
    /** @brief Вычисляет матрицу вида (LookAt). */
    glm::mat4 GetViewMatrix() const;

    /** @brief Вычисляет матрицу проекции (Perspective). */
    glm::mat4 GetProjectionMatrix(float width, float height) const;
    ///@}

    // Обработка движения (WASD)
    void ProcessKeyboard(const std::string& direction, float deltaTime);

    /**
     * @brief Поворот камеры на основе смещения мыши.
     * @param xoffset Смещение по оси X.
     * @param yoffset Смещение по оси Y.
     * @param constrainPitch Если true, не дает камере "перевернуться" через зенит.
     */
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    // Геттеры для позиции (пригодятся для рендеринга чанков)
    glm::vec3 GetPosition() const { return m_Position; }
    void SetPosition(const glm::vec3& position) { m_Position = position; UpdateCameraVectors(); }

    glm::vec3 GetForward() const { return m_Front; }

    float& GetSensitivity() { return m_MouseSensitivity; } // Возвращаем ссылку для ImGui
    void SetSensitivity(float s) { m_MouseSensitivity = s; }

    float& GetMovementSpeed() { return m_MovementSpeed; }
    void SetMovementSpeed(float new_MovementSpeed ) { m_MovementSpeed = new_MovementSpeed; }

    float& GetFOV() { return m_FOV; }
    void SetFOV(float fov) { m_FOV = fov; }

    /**
     * @brief Инициализирует подписки на события ввода.
     * * Связывает MouseScrolledEvent с зумом и KeyEvents с массивом состояний.
     */
    void InitEvents();

    /**
     * @brief Обновление позиции камеры каждый кадр.
     * * Обрабатывает зажатые клавиши из m_Keys для плавного движения.
     * @param deltaTime Время кадра для независимой от FPS скорости.
     */
    void OnUpdate(float deltaTime);
private:
    /**
     * @brief Пересчитывает векторы Front, Right и Up на основе углов Эйлера.
     * * Вызывается каждый раз при изменении Yaw или Pitch.
     */
    void UpdateCameraVectors();

private:
    glm::vec3 m_Position; ///< Позиция камеры в мировых координатах.ы
    glm::vec3 m_Front;    ///< Вектор направления взгляда.
    glm::vec3 m_Up;       ///< Локальный вектор "вверх" для камеры.
    glm::vec3 m_Right;    ///< Вектор "вправо" (для стрейфа).
    glm::vec3 m_WorldUp;  ///< Глобальный вектор верха.

    float m_Yaw;          ///< Угол рыскания (горизонтальный).
    float m_Pitch;        ///< Угол тангажа (вертикальный).

    float m_MovementSpeed = 15.0f;    ///< Скорость полета.
    float m_MouseSensitivity = 0.1f;  ///< Чувствительность мыши.
    float m_FOV = 70.0f;             ///< Угол обзора (в градусах).

    bool m_Keys[1024] = { false };   ///< Карта состояний клавиш клавиатуры.
};
