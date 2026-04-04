# ==============================================================================
# PROJECT: Fluxborn Engine
# ROLE:    External Dependencies Layer (The Foundation)
# ==============================================================================

cmake_minimum_required(VERSION 3.20)

# --- 1. ГЛОБАЛЬНЫЕ НАСТРОЙКИ СЛОЯ ---
set(EXTERNAL_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake_options_libs")

# Группировка всех внешних библиотек в IDE
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# --- 2. GLFW (Windowing & Context) ---
include("${EXTERNAL_CONFIG_DIR}/glfw_config.cmake")

# Решение проблемы путей генерации для GLFW
set(GLFW_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/glfw" CACHE INTERNAL "")

add_subdirectory(glfw)
add_library(external::glfw ALIAS glfw)

# Силовой проброс инклюдов для сборки самого GLFW
target_include_directories(glfw PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/glfw/src")

message(STATUS "[External] GLFW target ready (Static).")


# --- 3. GLAD (OpenGL Loader) ---
add_subdirectory(glad)
add_library(external::glad ALIAS glad)

message(STATUS "[External] GLAD target ready (Static).")


# --- 4. GLM (Mathematics) ---
include("${EXTERNAL_CONFIG_DIR}/glm_config.cmake")

add_subdirectory(glm)
add_library(external::glm ALIAS glm)
target_compile_definitions(glm INTERFACE ${FLUX_GLM_DEFINITIONS})

message(STATUS "[External] GLM target ready (AVX2 + C++20).")


# --- 5. IMGUI (Immediate Mode UI) ---
add_subdirectory(imgui)
add_library(external::imgui ALIAS imgui)

message(STATUS "[External] ImGui target ready.")


# --- 6. STB (Media Loading) ---
# Форсируем сборку реализаций в отдельную статику
include("${EXTERNAL_CONFIG_DIR}/stb_config.cmake")

add_subdirectory(stb)
target_compile_definitions(stb PRIVATE ${FLUX_STB_DEFINITIONS})
add_library(external::stb ALIAS stb)

message(STATUS "[External] STB target ready (Static implementations).")


# --- 7. ENTT (Entity Component System) ---
include("${EXTERNAL_CONFIG_DIR}/entt_config.cmake")

# EnTT — Header-only, создаем интерфейсный таргет
add_library(entt INTERFACE)
target_include_directories(entt INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/entt/src")
add_library(external::entt ALIAS entt)
target_compile_definitions(entt INTERFACE ${FLUX_ENTT_DEFINITIONS})

message(STATUS "[External] EnTT target ready (Header-only).")


# --- 8. ЗАВЕРШЕНИЕ ---
message(STATUS "-------------------------------------------------------")
message(STATUS "[External] All submodules integrated successfully.")
message(STATUS "-------------------------------------------------------")
