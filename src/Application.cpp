/**
 * @file Application.cpp
 * @brief Реализация методов класса Application.
 */

#include "Application.hpp"

#include "Core/Events/EventManager.h"
#include "Core/Events/MouseEvents.h"
#include "Core/Events/KeyboardEvents.h"

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
    // 1. Сначала шейдер!
    m_TestShader = std::make_unique<Shader>("Assets/shaders/default.vert", "Assets/shaders/default.frag");

    // Загружаем атлас (путь должен вести к твоему PNG с 16x16 блоками)
    m_BlockAtlas = std::make_unique<Texture>("Assets/textures/terrain.png");

    // 2. Потом камера
    m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 15.0f, 30.0f));
    // ПОДПИСКА КАМЕРЫ НА СОБЫТИЯ
    EventManager::getInstance().subscribe<MouseMovedEvent>([this](MouseMovedEvent& e) {
        if (m_CursorLocked) {
            // Рассчитываем дельту прямо здесь
            float xoffset = e.getX() - m_LastX;
            float yoffset = m_LastY - e.getY(); // перевернуто, так как y-координаты идут снизу вверх

            m_LastX = e.getX();
            m_LastY = e.getY();

            // Защита от скачков при захвате курсора
            if (std::abs(xoffset) < 500.0f && std::abs(yoffset) < 500.0f) {
                m_Camera->ProcessMouseMovement(xoffset, yoffset);
            }
        } else {
            // Если курсор не заблокирован, просто обновляем координаты, чтобы не было скачка при входе
            m_LastX = e.getX();
            m_LastY = e.getY();
        }
    });
    m_Camera->InitEvents(); // ДОБАВЬ ЭТУ СТРОКУ




    int new_seed;//пускай будет забита мусором для рандомной генерации
    // 3. И только в конце мир (так как он сразу генерирует меши и требует контекст)
    m_World = std::make_unique<World>(new_seed);
    m_World->GenerateRegion(24);
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



    m_LastX = m_Width / 2.0f;
    m_LastY = m_Height / 2.0f;

    // НОВОЕ: Синхронизируем состояние курсора с начальным значением m_CursorLocked
    if (m_CursorLocked) {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    glfwSetWindowUserPointer(m_Window, this); // Сохраняем указатель на класс в окне
    glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);


    int fbWidth, fbHeight;
    glfwGetFramebufferSize(m_Window, &fbWidth, &fbHeight);
    m_Width = fbWidth;
    m_Height = fbHeight;
    glViewport(0, 0, m_Width, m_Height);



    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); // Включаем отсечение граней
    glEnable(GL_BLEND);
    glCullFace(GL_BACK);    // Не рисуем задние
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    //Я просто закомментировал имена неиспользуемых переменных, например GLFWwindow* /*window*/.
    //Для компилятора это сигнал: "Я знаю, что этот параметр тут есть, но он мне не нужен"
    // 1. Движение мыши
    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* /*window*/, double xpos, double ypos) {
        EventManager::getInstance().emit<MouseMovedEvent>((float)xpos, (float)ypos);
    });

    // 1.1 Скрол мыши
    glfwSetScrollCallback(m_Window, [](GLFWwindow* /*window*/, double xoffset, double yoffset) {
        EventManager::getInstance().emit<MouseScrolledEvent>((float)xoffset, (float)yoffset);
    });

    // 2. Клавиатура
    glfwSetKeyCallback(m_Window, [](GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int mods) {
        auto& em = EventManager::getInstance();
        if (action == GLFW_PRESS)
            em.emit<KeyPressedEvent>(key, mods, false);
        else if (action == GLFW_RELEASE)
            em.emit<KeyReleasedEvent>(key, mods);
        else if (action == GLFW_REPEAT)
            em.emit<KeyPressedEvent>(key, mods, true);
    });

    // 3. Кнопки мыши
    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int /*mods*/) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        if (action == GLFW_PRESS)
            EventManager::getInstance().emit<MouseButtonPressedEvent>(button, (float)x, (float)y);
        else
            EventManager::getInstance().emit<MouseButtonReleasedEvent>(button, (float)x, (float)y);
    });
}


void Application::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    // Получаем указатель на экземпляр приложения через user pointer
    auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->m_Width = width;
        app->m_Height = height;
        glViewport(0, 0, width, height);
    }
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

        // РАЗГРЕБАЕМ ОЧЕРЕДЬ СОБЫТИЙ
        EventManager::getInstance().processQueue();

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
    // Закрытие на ESC
    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        m_Running = false;

    // 1. ОСВОБОЖДЕНИЕ (Alt)
    if (glfwGetKey(m_Window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && m_CursorLocked) {
        m_CursorLocked = false;
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    // 2. ЗАХВАТ (Клик)
    if (glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!m_CursorLocked && !ImGui::GetIO().WantCaptureMouse) {
            m_CursorLocked = true;

            // КРИТИЧЕСКИЙ МОМЕНТ ДЛЯ УБРАНИЯ СКАЧКА:
            double x, y;
            glfwGetCursorPos(m_Window, &x, &y);
            m_LastX = (float)x; // Фиксируем положение ПЕРЕД скрытием
            m_LastY = (float)y;
            m_FirstMouse = false; // Мы уже установили координаты, "первый кадр" не нужен

            glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }

    // 3. ОБРАБОТКА (Вращение)
    // if (m_CursorLocked) {
    //     double xpos, ypos;
    //     glfwGetCursorPos(m_Window, &xpos, &ypos);
    //
    //     // Рассчитываем дельту
    //     float xoffset = (float)xpos - m_LastX;
    //     float yoffset = m_LastY - (float)ypos;
    //
    //     // Обновляем старые координаты
    //     m_LastX = (float)xpos;
    //     m_LastY = (float)ypos;
    //
    //     // Если смещение слишком огромное (например, больше 100 пикселей за кадр),
    //     // игнорируем его — это защитит от багов при переключении окон
    //     if (std::abs(xoffset) < 500.0f && std::abs(yoffset) < 500.0f) {
    //         m_Camera->ProcessMouseMovement(xoffset, yoffset);
    //     }
    // }
}

/**
 * @brief Обновление логики приложения и ECS систем.
 * @param deltaTime Время кадра для синхронизации скоростей.
 */
void Application::Update(float deltaTime) {
    // Камера сама знает, нажаты ли клавиши, благодаря подписке
    if (m_CursorLocked) {
        m_Camera->OnUpdate(deltaTime);
    }

    // 2. БАЗОВАЯ КОЛЛИЗИЯ (оставляем здесь, так как она зависит от World)
    glm::vec3 pos = m_Camera->GetPosition();
    int groundHeight = m_World->GetHeightAt((int)std::floor(pos.x), (int)std::floor(pos.z));
    float playerHeight = 1.8f;
    if (pos.y < (float)groundHeight + playerHeight) {
        pos.y = (float)groundHeight + playerHeight;
        m_Camera->SetPosition(pos);
    }

}

/**
 * @brief Основная процедура рендеринга.
 * * Очищает буферы, подготавливает новый кадр ImGui, отрисовывает 3D/2D мир
 * и выводит отладочную информацию поверх.
 */
void Application::Render() {
    // 1. Подготовка кадра
    glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2. Рендеринг 3D мира
    m_TestShader->Bind(); // Используем поле класса вместо static
    // Биндим текстуру в 0-й слот
    m_BlockAtlas->Bind(0);
    // Сообщаем шейдеру, что семплер u_Texture должен брать данные из 0-го слота
    m_TestShader->SetInt("u_Texture", 0);


    glm::mat4 view = m_Camera->GetViewMatrix();
    glm::mat4 proj = m_Camera->GetProjectionMatrix((float)m_Width, (float)m_Height);


    m_TestShader->SetMat4("u_ViewProjection", proj * view);
    m_TestShader->SetVec4("u_Color", m_CubeColor);
    m_TestShader->SetFloat("u_LightIntensity", m_LightIntensity); // Передаем силу
    m_TestShader->SetVec3("u_LightDir", glm::normalize(m_LightDir)); // И направление

    // Рисуем все чанки
    m_World->Render(m_TestShader.get());

    m_TestShader->Unbind();
    // --- КОНЕЦ ТЕСТОВОГО БЛОКА ---

    // 2. Рендеринг ImGui интерфейса
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 1. Рассчитываем размеры динамически на каждом кадре
    float panelWidth = (float)m_Width / 3.0f;
    if (panelWidth < 250.0f) panelWidth = 250.0f; // Ограничение минимальной ширины

    // Привязываем окно к правому верхнему углу
    ImGui::SetNextWindowPos(ImVec2((float)m_Width - panelWidth, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panelWidth, (float)m_Height), ImGuiCond_Always);

    // Флаги окна (убираем NoResize, так как мы сами управляем размером через SetNextWindowSize)
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
    ImGui::Begin("Fluxborn Debug Panel", nullptr, windowFlags);
    {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "System Info");
        ImGui::Text("FPS: %.1f (%.3f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

        ImGui::Separator();
        ImGui::Text("Cursor Status: "); ImGui::SameLine();
        if (m_CursorLocked) ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "LOCKED (Game)");
        else ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "FREE (UI)");

        ImGui::SeparatorText("Camera");
        glm::vec3 cPos = m_Camera->GetPosition();
        ImGui::Text("Pos: %.1f, %.1f, %.1f", cPos.x, cPos.y, cPos.z);

        ImGui::SeparatorText("Controls");
        ImGui::BulletText("W, A, S, D - Move");
        ImGui::BulletText("Space / L-Ctrl - Up / Down");
        ImGui::BulletText("Left Alt - Unlock Mouse");
        ImGui::BulletText("L-Click Window - Lock Mouse");

        ImGui::SeparatorText("Settings");

        // Слайдер для скорости передвижения
        ImGui::SliderFloat("Movement Speed", &m_Camera->GetMovementSpeed(), 1.0f, 100.0f, "%.1f");

        // Настройка чувствительности (уже была)
        ImGui::SliderFloat("Mouse Sensitivity", &m_Camera->GetSensitivity(), 0.01f, 1.0f, "%.2f");

        if (ImGui::Button("Reset Settings")) {
            m_Camera->GetMovementSpeed() = 10.0f; // Значение по умолчанию
            m_Camera->SetSensitivity(0.1f);
        }
        ImGui::SeparatorText("Camera Lens");

        // Слайдер FOV (Field of View)
        if (ImGui::SliderFloat("FOV", &m_Camera->GetFOV(), 30.0f, 120.0f, "%.0f deg")) {
            // Если нужно что-то делать при изменении FOV, можно добавить логику тут
        }

        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Угол обзора: больше - видишь больше, меньше - эффект зума");

        // Кнопка быстрого сброса к стандартному значению (70 - стандарт Minecraft)
        if (ImGui::Button("Standard FOV (70)")) {
            m_Camera->SetFOV(70.0f);
        }

        ImGui::SeparatorText("Texture Info");
        ImGui::Text("Atlas: %dx%d", m_BlockAtlas->GetWidth(), m_BlockAtlas->GetHeight());
        // Можно даже отрисовать саму текстуру в окне ImGui (нужно кастануть ID к ImTextureID)
        ImGui::Image((void*)(intptr_t)m_BlockAtlas->GetRendererID(), ImVec2(128, 128));

        ImGui::SameLine();
        if (ImGui::Button("Quake Pro (110)")) {
            m_Camera->SetFOV(110.0f);
        }

        ImGui::SeparatorText("Appearance");
        ImGui::ColorEdit4("Cube Color", glm::value_ptr(m_CubeColor));

        ImGui::SeparatorText("Lighting");
        ImGui::SliderFloat("Ambient Intensity", &m_LightIntensity, 0.0f, 2.0f);
        ImGui::SliderFloat3("Light Direction", glm::value_ptr(m_LightDir), -1.0f, 1.0f);
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



    // 3. Смена буферов и опрос событий
    glfwSwapBuffers(m_Window);
    glfwPollEvents();
}

/**
 * @brief Корректное завершение работы.
 * * Выполняется деинициализация ImGui, закрытие окна и деинициализация GLFW.
 */
void Application::Shutdown() {
    m_World.reset();       // Удаляем чанки и их Mesh (ВАО/VBO) пока жив контекст
    m_TestShader.reset();  // Удаляем шейдер

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (m_Window) {
        glfwDestroyWindow(m_Window);
    }
    glfwTerminate();
}
