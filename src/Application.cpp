/**
 * @file Application.cpp
 * @brief Реализация методов класса Application.
 */

#include "Application.hpp"
#include "Renderer/Shader.hpp" // Теперь подключаем здесь!
#include "Renderer/VertexArray.hpp"
#include "Renderer/Mesh.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <stdexcept>

/**
 * @brief Создает объект приложения и запускает базовую инициализацию.
 */
Application::Application(const std::string& title, int width, int height)
: m_Title(title), m_Width(width), m_Height(height)
{
    InitGLFW();
    InitImGui();
    // ОБЯЗАТЕЛЬНО инициализируем камеру, иначе будет вылет
    m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
}

/**
 * @brief Деструктор автоматически вызывает Shutdown() для освобождения ресурсов.
 */
Application::~Application() {
    Shutdown();
}

/**
 * @brief Инициализация подсистемы GLFW и загрузка расширений OpenGL через GLAD.
 * * Настраивает параметры окна (Hints) для OpenGL 4.5 Core Profile.
 * Включает V-Sync для ограничения частоты кадров под монитор.
 * * @throw std::runtime_error Если не удалось инициализировать библиотеку или создать окно.
 */
void Application::InitGLFW() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    // Настройка контекста OpenGL 4.5
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
    if (!m_Window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1); // Включение вертикальной синхронизации (V-Sync)

    // Загрузка указателей на функции OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glEnable(GL_DEPTH_TEST);
}

/**
 * @brief Настройка интерфейса отладки ImGui.
 * * Устанавливает темную тему и включает поддержку Docking (прилипание окон).
 * Инициализирует бэкенды для работы с GLFW и OpenGL 3.
 */
void Application::InitImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Разрешаем перетаскивание окон ImGui друг в друга
    io.IniFilename = nullptr; // Полностью отключить сохранение настроек

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

/**
 * @brief Главный игровой цикл.
 * * Рассчитывает deltaTime между кадрами для обеспечения независимой от FPS логики.
 */
void Application::Run() {
    float lastFrame = 0.0f;

    while (m_Running && !glfwWindowShouldClose(m_Window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ProcessInput();
        Update(deltaTime);
        Render();
    }
}

/**
 * @brief Обработка пользовательского ввода.
 * * @note На данный момент обрабатывает только закрытие приложения по нажатию ESC.
 */
void Application::ProcessInput() {
    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        m_Running = false;
}

/**
 * @brief Обновление логики приложения и ECS систем.
 * @param deltaTime Время кадра для синхронизации скоростей.
 */
void Application::Update(float deltaTime) {
    // TODO: Интегрировать системы EnTT (движение, физика, AI)
    if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS) m_Camera->ProcessKeyboard("FORWARD", deltaTime);
    if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS) m_Camera->ProcessKeyboard("BACKWARD", deltaTime);
    if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS) m_Camera->ProcessKeyboard("LEFT", deltaTime);
    if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS) m_Camera->ProcessKeyboard("RIGHT", deltaTime);
}

/**
 * @brief Основная процедура рендеринга.
 * * Очищает буферы, подготавливает новый кадр ImGui, отрисовывает 3D/2D мир
 * и выводит отладочную информацию поверх.
 */
void Application::Render() {
    glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Добавь очистку буфера глубины для 3D!

    static std::unique_ptr<Shader> testShader;
    static std::unique_ptr<Mesh> cubeMesh; // Используем Mesh вместо vao/vbo
    static bool initialized = false;
    static float triangleColor[4] = { 0.8f, 0.4f, 0.1f, 1.0f };

    if (!initialized) {
        testShader = std::make_unique<Shader>("Assets/shaders/default.vert", "Assets/shaders/default.frag");

        // Данные куба (позиции)
        std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f,  0.5f}}, {{ 0.5f, -0.5f,  0.5f}}, {{ 0.5f,  0.5f,  0.5f}}, {{-0.5f,  0.5f,  0.5f}},
            {{-0.5f, -0.5f, -0.5f}}, {{ 0.5f, -0.5f, -0.5f}}, {{ 0.5f,  0.5f, -0.5f}}, {{-0.5f,  0.5f, -0.5f}}
        };
        // Индексы
        std::vector<uint32_t> indices = {
            0, 1, 2, 2, 3, 0, 1, 5, 6, 6, 2, 1, 7, 6, 5, 5, 4, 7,
            4, 0, 3, 3, 7, 4, 4, 5, 1, 1, 0, 4, 3, 2, 6, 6, 7, 3
        };

        cubeMesh = std::make_unique<Mesh>(vertices, indices);
        initialized = true;
    }

    testShader->Bind();

    glm::mat4 view = m_Camera->GetViewMatrix();
    glm::mat4 proj = m_Camera->GetProjectionMatrix((float)m_Width, (float)m_Height);
    glm::mat4 model = glm::mat4(1.0f);

    testShader->SetMat4("u_Model", model);
    testShader->SetMat4("u_ViewProjection", proj * view);
    testShader->SetVec4("u_Color", glm::make_vec4(triangleColor));

    cubeMesh->Draw(); // Красиво и коротко

    testShader->Unbind();
    // --- КОНЕЦ ТЕСТОВОГО БЛОКА ---

    // ImGui... (оставляем как есть)
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Debug Info");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::ColorEdit4("Triangle Color", triangleColor);
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_Window);
    glfwPollEvents();
}

/**
 * @brief Корректное завершение работы.
 * * Выполняется деинициализация ImGui, закрытие окна и деинициализация GLFW.
 */
void Application::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (m_Window) {
        glfwDestroyWindow(m_Window);
    }
    glfwTerminate();
}
