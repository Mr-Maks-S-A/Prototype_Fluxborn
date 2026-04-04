# External/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# 1. Путь к конфигам
set(EXTERNAL_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake_options_libs")

# 2. Конфигурация GLFW (твои CACHE FORCE переменные)
include("${EXTERNAL_CONFIG_DIR}/glfw_config.cmake")

# --- ВОТ ЭТОТ БЛОК РЕШАЕТ ПРОБЛЕМУ ---
# Мы явно указываем GLFW, где генерировать конфиг,
# чтобы он не запутался в твоей структуре папок.
set(GLFW_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/glfw" CACHE INTERNAL "")

add_subdirectory(glfw)

# 3. Силовой проброс пути к инклюдам (чтобы init.c его увидел)
if(TARGET glfw)
    target_include_directories(glfw PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/glfw/src")
endif()


message(STATUS "[External] All submodules integrated into the build tree.")
