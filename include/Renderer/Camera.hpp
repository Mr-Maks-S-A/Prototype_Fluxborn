/**
 * @file Camera.hpp
 * @brief Класс камеры для навигации в 3D пространстве.
 */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

class Camera {
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f, float pitch = 0.0f);

    // Матрица Вида (View Matrix)
    glm::mat4 GetViewMatrix() const;

    // Матрица Проекции (Projection Matrix)
    glm::mat4 GetProjectionMatrix(float width, float height) const;

    // Обработка движения (WASD)
    void ProcessKeyboard(const std::string& direction, float deltaTime);

    // Обработка поворота мыши
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    // Геттеры для позиции (пригодятся для рендеринга чанков)
    glm::vec3 GetPosition() const { return m_Position; }
    glm::vec3 GetForward() const { return m_Front; }

private:
    void UpdateCameraVectors();

private:
    // Векторы положения
    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;

    // Углы Эйлера
    float m_Yaw;
    float m_Pitch;

    // Настройки камеры
    float m_MovementSpeed = 5.0f;
    float m_MouseSensitivity = 0.1f;
    float m_Zoom = 45.0f;
};
