# ==============================================================================
# PROJECT: Fluxborn Engine
# ROLE:    Root Configuration (The Orchestrator)
# ==============================================================================
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# Инициализация проекта
project(Fluxborn
    VERSION 0.1.0
    DESCRIPTION "Modular Voxel-based Engine Prototype"
    LANGUAGES CXX C
)


# --- 1. ГЛОБАЛЬНЫЕ ТЕХНИЧЕСКИЕ НАСТРОЙКИ ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Чистый C++, без расширений компилятора

# Экспорт базы данных команд компиляции (для работы LSP в VS Code/Neovim)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# --- 2. ПОДКЛЮЧЕНИЕ МОДУЛЕЙ НАСТРОЙКИ ---
# Добавляем папку Cmake в пути поиска модулей
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/Cmake")

# Запускаем наш определитель платформы, архитектуры и путей вывода
include(SetupPlatform)


# --- 3. ИНИЦИАЛИЗАЦИЯ ИНФРАСТРУКТУРЫ ---

# Создаем базовые директории, если их нет (опционально, CMake сделает это сам)
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/logs")


# --- 4. ПОДКЛЮЧЕНИЕ СЛОЕВ (ПОРЯДОК КРИТИЧЕН) ---

# Слой 1: Внешние библиотеки
# Сначала собираем фундамент, чтобы Internal и Fluxborn видели таргеты (glfw, glad и т.д.)
add_subdirectory(External)

# Слой 2: Внутренние системные модули (Движок)
# Здесь собирается UntilWindow и другие системные части
# add_subdirectory(Internal)

# Слой 3: Конечное приложение (Игра)
# Основной исполняемый файл, линкующий всё вышеперечисленное
add_subdirectory(Fluxborn)


# --- 5. ФИНАЛЬНЫЙ СТАТУС ---
message(STATUS "=======================================================")
message(STATUS " Fluxborn Build Configuration Summary:")
message(STATUS "  - Target:   ${FLUX_OS}_${FLUX_ARCH}")
message(STATUS "  - Compiler: ${FLUX_COMPILER}")
message(STATUS "  - Config:   ${FLUX_BUILD_CONFIG}")
message(STATUS "  - Binaries: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "=======================================================")
