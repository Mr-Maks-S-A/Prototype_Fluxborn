#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

// Структура-компонент для проверки EnTT
struct Tag {
    std::string name;
};

int main() {
    // 1. Инициализация GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Настройка версии OpenGL (4.5 Core)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Создание окна
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Fluxborn Prototype | Tech Check", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // V-Sync ON

    // 2. Инициализация GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Вывод информации о железе
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    // 3. Инициализация ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Включаем докинг

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");

    // 4. Проверка EnTT
    entt::registry registry;
    auto entity = registry.create();
    registry.emplace<Tag>(entity, "VoxelEngine_Core");

    // Основной цикл
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Начало кадра ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Рендеринг фона (OpenGL)
        glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Окно отладки ImGui
        {
            ImGui::Begin("Fluxborn | Debug Info");

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / io.Framerate, io.Framerate);

            ImGui::Separator();
            ImGui::Text("ECS Status: Entity created with Tag: %s",
                        registry.get<Tag>(entity).name.c_str());

            ImGui::Separator();
            ImGui::Text("GPU: %s", glGetString(GL_RENDERER));
            ImGui::Text("OpenGL: %s", glGetString(GL_VERSION));

            if (ImGui::Button("Exit Game")) {
                glfwSetWindowShouldClose(window, true);
            }

            ImGui::End();
        }

        // Завершение кадра ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Очистка ресурсов
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
