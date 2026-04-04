# ==============================================================================
# PROJECT: Fluxborn Engine
# ROLE:    End-User Application Layer (The Bridge)
# ==============================================================================

# --- 1. ОПРЕДЕЛЕНИЕ ИСХОДНОГО КОДА ---
# Группируем файлы по логическим модулям для чистоты
set(FLUX_APP_SOURCES
    "src/main.cpp"
    "src/Application.cpp"
#
    # Core & Systems
    "src/Core/Events/EventManager.cpp"

    # Renderer Subsystem
    "src/Renderer/Buffer.cpp"
    "src/Renderer/Camera.cpp"
    "src/Renderer/Mesh.cpp"
    "src/Renderer/Shader.cpp"
    "src/Renderer/Texture.cpp"
    "src/Renderer/VertexArray.cpp"

    # World & Voxel Logic
    "src/World/Chunk.cpp"
    "src/World/World.cpp"
)

# --- 2. СОЗДАНИЕ ИСПОЛНЯЕМОГО ФАЙЛА ---
add_executable(FluxbornApp ${FLUX_APP_SOURCES})

# Настройка псевдонима для единообразия (удобно для тестов или внешних инструментов)
add_executable(Fluxborn::App ALIAS FluxbornApp)


# --- 3. ИНКЛЮДЫ И ЗАГОЛОВКИ ---
target_include_directories(FluxbornApp PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)


# --- 4. ЛИНКОВКА (MODERN CMAKE) ---
# Используем таргеты из слоя External и Internal.
# Зависимости транзитивно подтянут нужные инклюды и флаги.
target_link_libraries(FluxbornApp PRIVATE
    # Системные слои (когда активируешь Internal)
    # Fluxborn::Internal

    # Внешние библиотеки
    external::glfw
    external::glm   # Хорошим тоном считается использование namespace-таргетов, если они есть
    external::entt
    external::stb
    external::glad
    external::imgui

    # Системные зависимости
    ${CMAKE_DL_LIBS}
)

# --- 5. СПЕЦИФИЧНЫЕ НАСТРОЙКИ (DEFINITIONS) ---
# Пробрасываем путь к ассетам прямо в C++ код
target_compile_definitions(FluxbornApp PRIVATE
    FLUX_ASSETS_PATH="$<TARGET_FILE_DIR:FluxbornApp>/Assets/"
)


# --- 6. СВОЙСТВА ТАРГЕТА ---
set_target_properties(FluxbornApp PROPERTIES
    OUTPUT_NAME "Fluxborn"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    FOLDER "Applications"
)


# --- 7. ПОСТ-БОРКА (РЕСУРСЫ И АССЕТЫ) ---
# Кроссплатформенное копирование ассетов в бинарную директорию
add_custom_command(TARGET FluxbornApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/Assets"
    "$<TARGET_FILE_DIR:FluxbornApp>/Assets"
    COMMENT ">>> [Fluxborn] Syncing assets to output directory..."
)


# --- 8. ФИНАЛЬНЫЙ СТАТУС МОДУЛЯ ---
message(STATUS "[Module] Fluxborn App: Configured for ${FLUX_OS} (${FLUX_ARCH})")
