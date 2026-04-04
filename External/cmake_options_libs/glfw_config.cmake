# ==============================================================================
# GLFW 3.4 Configuration Module
# Project: Fluxborn Engine
# Description: Detailed configuration for cross-platform window management.
# ==============================================================================


# --- 0. АВТООПРЕДЕЛЕНИЕ ВЕРСИИ ---
# Мы извлекаем версию напрямую из подмодуля, чтобы логи всегда были актуальны.
# Файл CMakeLists.txt в GLFW содержит строку project(GLFW VERSION ...)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/glfw/CMakeLists.txt")
    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/glfw/CMakeLists.txt" GLFW_PROJECT_LINE REGEX "project\\(GLFW VERSION ([0-9.]+)")
    string(REGEX REPLACE ".*VERSION ([0-9.]+).*" "\\1" FLUX_GLFW_VERSION "${GLFW_PROJECT_LINE}")
else()
    set(FLUX_GLFW_VERSION "Unknown")
endif()



# --- 1. ОПТИМИЗАЦИЯ И ЧИСТОТА СБОРКИ (BUILD FLOW) ---

# [EXAMPLES] Нам не нужны бинарники демонстрационных программ GLFW в папке build.
# Это экономит время компиляции и место на диске.
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Disable GLFW bundled examples" FORCE)

# [TESTS] Отключаем внутренние тесты GLFW. Качество библиотеки проверено сообществом,
# а лишние проверки замедляют CI/CD и локальную разработку.
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Disable GLFW internal test suite" FORCE)

# [DOCS] Отключаем генерацию документации Doxygen. Все API доступно онлайн,
# и локальная сборка мануалов в процессе компиляции движка не требуется.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Disable GLFW documentation generation" FORCE)

# [INSTALL] Критически важно: отключаем генерацию цели 'install'.
# Мы управляем зависимостями локально внутри проекта. Установка GLFW в системные
# каталоги (/usr/lib или /usr/local) может привести к конфликту с пакетным менеджером (pacman).
set(GLFW_INSTALL OFF CACHE BOOL "Prevent GLFW from installing globally" FORCE)

# [STATIC LIBRARIES] Собираем GLFW как статическую библиотеку (.a / .lib).
# Это вшивает код GLFW прямо в наш бинарник. Удобно для деплоя: не нужно таскать
# за собой ворох .so или .dll файлов, и линковка работает быстрее.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Force static linking for GLFW" FORCE)


# --- 2. LINUX / UNIX SPECIFIC (CachyOS/Arch Compatibility) ---

# --- LINUX (CachyOS / Arch) ---
if(UNIX AND NOT APPLE)
    message(STATUS "[GLFW] Backend: Hybrid Wayland + X11 (Auto-detected version: ${FLUX_GLFW_VERSION})")

    # Включаем оба бэкенда. GLFW 3.4+ сам выберет лучший при запуске (обычно Wayland).
    set(GLFW_BUILD_WAYLAND ON CACHE BOOL "Enable Wayland support" FORCE)
    set(GLFW_BUILD_X11     ON CACHE BOOL "Enable X11 support" FORCE)

    # [RUNTIME LIBRARIES] GLFW будет загружать libwayland-client.so и libX11.so
    # динамически через dlopen, что делает бинарник переносимым.
    set(GLFW_VULKAN_STATIC OFF CACHE BOOL "Disable static Vulkan linking" FORCE)
endif()

# --- macOS (Cocoa) ---
if(APPLE)
    message(STATUS "[GLFW] Backend: Cocoa (macOS Native)")
    # Отключаем использование старого кода (Retina включена по умолчанию в 3.4)
    set(GLFW_COCOA_RETINA ON CACHE BOOL "Support for High-DPI on macOS" FORCE)
    set(GLFW_COCOA_CHDIR  ON CACHE BOOL "Change to resources directory on startup" FORCE)
endif()

# --- WINDOWS (MSVC) ---
if(MSVC)
    # [RUNTIME] Синхронизируем рантайм. Если Fluxborn собран с /MD, GLFW обязан быть /MD.
    set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE BOOL "Match MSVC runtime" FORCE)
endif()

# Глобальный макрос для GLFW (полезно для отладки)
add_definitions(-D_GLFW_USE_CONFIG_H)

# --- 4. МАКРОСЫ И ГЛОБАЛЬНЫЕ ОПРЕДЕЛЕНИЯ ---

# [_GLFW_USE_CONFIG_H]
# Этот макрос заставляет GLFW включать внутренний файл 'glfw_config.h',
# сгенерированный CMake. Это критично, чтобы код GLFW знал, что мы включили Wayland.
# ВАЖНО: Мы используем add_compile_definitions, чтобы это применилось ТОЛЬКО
# к таргету GLFW при его сборке, не загрязняя наш код.
# add_definitions(-D_GLFW_USE_CONFIG_H)



# --- 5. FINALIZATION ---

# Сообщаем системе сборки, что переменные зафиксированы.
# ПРЕДУПРЕЖДЕНИЕ: Макрос _GLFW_USE_CONFIG_H добавляется самой библиотекой GLFW
# автоматически при обнаружении CMake. Мы не пробрасываем его глобально,
# чтобы не ломать инклюды в наших Internal модулях.

message(STATUS "[External] GLFW ${FLUX_GLFW_VERSION}: Configuration locked.")
