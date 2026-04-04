/**
 * @file Application.hpp
 * @brief Главный класс управления жизненным циклом приложения Fluxborn.
 * @author Maxim (Fluxborn Developer)
 * @date 2026-03-28
 */

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <entt/entt.hpp>
#include "Renderer/Texture.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/Shader.hpp" // Теперь подключаем здесь!
#include "World/World.hpp"

#include <string>
#include <memory>

/**
 * @class Application
 * @brief Базовый класс, инкапсулирующий окно, контекст рендеринга и игровой цикл.
 * * Класс реализует паттерн Singleton (опционально) или просто владеет основными подсистемами.
 * Отвечает за инициализацию графического API (OpenGL 4.5), интеграцию ImGui и
 * управление реестром сущностей EnTT.
 */
class Application {
public:
    /**
     * @brief Конструктор приложения.
     * @param title Заголовок окна.
     * @param width Ширина окна в пикселях.
     * @param height Высота окна в пикселях.
     * @throw std::runtime_error Если инициализация GLFW или Glad завершилась ошибкой.
     */
    Application(const std::string& title, int width, int height);

    /**
     * @brief Деструктор. Выполняет корректную очистку ресурсов (Shutdown).
     */
    ~Application();

    /** @name Запрет копирования
     * Приложение является уникальным объектом и не может быть скопировано.
     */
    ///@{
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    ///@}

    /**
     * @brief Запускает главный цикл приложения.
     * * Метод содержит цикл while, который работает до закрытия окна или вызова Stop().
     * Внутри вызываются этапы: ProcessInput, Update и Render.
     */
    void Run();

    /**
     * @brief Инициирует немедленную остановку приложения.
     */
    void Stop();

private:
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

    /**
     * @brief Настройка GLFW и создание контекста OpenGL 4.5 Core.
     * @internal
     */
    void InitGLFW();

    /**
     * @brief Настройка контекста ImGui и привязка к текущему окну GLFW.
     * @internal
     */
    void InitImGui();

    /**
     * @brief Обработка системных событий ввода (клавиатура, мышь).
     */
    void ProcessInput();

    /**
     * @brief Логическое обновление состояния мира.
     * @param deltaTime Время, прошедшее с последнего кадра (в секундах).
     */
    void Update(float deltaTime);

    /**
     * @brief Рендеринг кадра (Clear -> Draw Call -> ImGui -> Swap).
     */
    void Render();

    /**
     * @brief Очистка ресурсов всех подсистем перед выходом.
     */
    void Shutdown();


private:
    GLFWwindow* m_Window = nullptr; ///< Указатель на нативное окно GLFW.
    std::string m_Title;            ///< Заголовок окна.
    int m_Width, m_Height;          ///< Текущие размеры окна.
    bool m_Running = true;          ///< Флаг активности главного цикла.

    glm::vec4 m_CubeColor = { 0.8f, 0.4f, 0.1f, 1.0f };

    std::unique_ptr<Texture> m_BlockAtlas;
    std::unique_ptr<Shader> m_TestShader;

    float m_LightIntensity = 1.0f; // Значение по умолчанию
    glm::vec3 m_LightDir = {0.5f, 1.0f, 0.3f}; // Направление света

    bool m_FirstMouse = true;
    float m_LastX = 0.0f;
    float m_LastY = 0.0f;
    bool m_CursorLocked = true;
    std::unique_ptr<Camera> m_Camera;

    std::unique_ptr<World> m_World;
    /** * @brief Реестр EnTT.
     * Хранит все сущности и компоненты (ECS).
     */
    entt::registry m_Registry;

    int m_DebugMode = 0; // 0: Standard, 1: Normals, 2: White


    bool m_EnableGravity = false;
    float m_VerticalVelocity = 0.0f;
    const float GRAVITY_CONSTANT = -20.0f; // Ускорение
    glm::vec3 m_SpawnPoint = glm::vec3(0.0f, 30.0f, 0.0f);
};
