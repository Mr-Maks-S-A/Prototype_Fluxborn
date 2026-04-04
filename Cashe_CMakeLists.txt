cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(Fluxborn
    VERSION 0.1.0
    DESCRIPTION "Voxel-based spellcasting game prototype"
    LANGUAGES CXX
)


# --- ГЛОБАЛЬНЫЕ НАСТРОЙКИ ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)




# --- ОПРЕДЕЛЕНИЕ КОМПИЛЯТОРА ---
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(FLUX_COMPILER "GCC")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(FLUX_COMPILER "Clang")
elseif(MSVC)
    set(FLUX_COMPILER "MSVC")
else()
    set(FLUX_COMPILER ${CMAKE_CXX_COMPILER_ID})
endif()

# Добавляем версию для еще большей точности (опционально)
set(FLUX_COMPILER "${FLUX_COMPILER}_${CMAKE_CXX_COMPILER_VERSION_MAJOR}")

# --- ОПРЕДЕЛЕНИЕ СИСТЕМНЫХ ПАРАМЕТРОВ ---
if(WIN32)
    set(FLUX_OS "Windows")
elseif(APPLE)
    set(FLUX_OS "macOS")
elseif(UNIX)
    set(FLUX_OS "Linux")
else()
    set(FLUX_OS "UnknownOS")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "(arm64|aarch64)")
        set(FLUX_ARCH "arm64")
    else()
        set(FLUX_ARCH "x64")
    endif()
else()
    set(FLUX_ARCH "x86")
endif()

# --- УПРАВЛЕНИЕ ВЫХОДНЫМИ ДИРЕКТОРИЯМИ ---
set(OUTPUT_BASE_DIR "${CMAKE_SOURCE_DIR}/build/bin/${FLUX_OS}/${FLUX_COMPILER}/${FLUX_ARCH}/$<CONFIG>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BASE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_BASE_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_BASE_DIR})

# --- ПОДКЛЮЧЕНИЕ CPM ---
# РЕАЛТАЙМ ПРОГРЕСС: Включаем отображение процентов скачивания в терминале
set(FETCHCONTENT_QUIET OFF CACHE BOOL "" FORCE)
# Форсируем использование SSH для всех загрузок через CPM
set(CPM_DOWNLOAD_PROTOCOL ssh CACHE STRING "Protocol used to download packages")
# Настраиваем кэш ПЕРЕД подключением CPM
set(CPM_SOURCE_CACHE "${CMAKE_SOURCE_DIR}/.cache/CPM" CACHE PATH "CPM source cache")
# Подключаем сам CPM
include("${CMAKE_SOURCE_DIR}/Cmake/CPM.cmake/cmake/CPM.cmake")

# --- ЗАВИСИМОСТИ ---

# --- GLFW 3.4 CONFIG ---
if(UNIX AND NOT APPLE)
    # Эти переменные теперь управляют сборкой бэкендов в 3.4
    set(GLFW_BUILD_WAYLAND ON CACHE BOOL "Build Wayland support")
    set(GLFW_BUILD_X11 ON CACHE BOOL "Build X11 support")
endif()

CPMAddPackage(
    NAME glfw
    GITHUB_REPOSITORY glfw/glfw
    GIT_TAG 3.4
    OPTIONS
        "GLFW_BUILD_EXAMPLES OFF"
        "GLFW_BUILD_TESTS OFF"
        "GLFW_BUILD_DOCS OFF"
        "GLFW_INSTALL OFF"
        "BUILD_SHARED_LIBS OFF"
        # Передаем правильные флаги в пакет
        "GLFW_BUILD_WAYLAND ${GLFW_BUILD_WAYLAND}"
        "GLFW_BUILD_X11 ${GLFW_BUILD_X11}"
)

# GLM (Версия 1.0.1 исправляет ошибки CMake на новых версиях)
CPMAddPackage(
    NAME glm
    GITHUB_REPOSITORY g-truc/glm
    GIT_TAG 1.0.1
)

# EnTT
CPMAddPackage(
    NAME entt
    GITHUB_REPOSITORY skypjack/entt
    GIT_TAG v3.12.2
    OPTIONS "ENTT_BUILD_TESTING OFF"
)

# --- STB (Image loading) ---
add_subdirectory("${CMAKE_SOURCE_DIR}/External/stb")

# --- ПОДКЛЮЧЕНИЕ ЛОКАЛЬНОГО GLAD ---
# Это заставит CMake зайти в твою папку и выполнить твой старый CMakeLists.txt
add_subdirectory("${CMAKE_SOURCE_DIR}/External/glad")

# --- ПОДКЛЮЧЕНИЕ ЛОКАЛЬНОГО imgui ---
# Это заставит CMake зайти в твою папку и выполнить твой старый CMakeLists.txt
add_subdirectory("${CMAKE_SOURCE_DIR}/External/imgui")

# --- ИСХОДНЫЕ ФАЙЛЫ ---
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/include/*.hpp")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# --- НАСТРОЙКА РАСШИРЕНИЯ И ИМЕНИ ---
if(WIN32)
    # В Windows добавляем .exe
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".exe")
else()
    # В Linux оставляем без расширения (стандарт)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".linux")
    #include <iostream>
    #ifdef __linux__
    #include <sys/utsname.h>
    #endif

    # void PrintSystemInfo() {
    #     #ifdef __linux__
    #     struct utsname buffer;
    #     if (uname(&buffer) == 0) {
    #         std::cout << "Running on: " << buffer.sysname << " " << buffer.release << std::endl;
    #     }
    #     #endif
    # }
endif()

# --- ИНТЕГРАЦИЯ IMGUI BACKENDS ---
# Важно: добавляем макрос IMGUI_IMPL_OPENGL_LOADER_GLAD, чтобы ImGui знал, как грузить функции
target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")

# --- ЛИНКОВКА ---
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glfw
        glm::glm
        EnTT::EnTT
        stb
        glad::glad  # Используем алиас
        imgui::imgui
        ${CMAKE_DL_LIBS} # ОБЯЗАТЕЛЬНО для Wayland/GLAD
)

# На Linux при статической сборке нужно явно линковать системные либы
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE m dl pthread X11)
endif()

# --- АССЕТЫ ---
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/Assets"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets"
    COMMENT "Syncing assets to output directory..."
)

# --- НАСТРОЙКИ КОМПИЛЯТОРА ---
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /utf-8)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
    FLUXBORN_ASSETS_DIR="$<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets/"
)


# Вывод в консоль для проверки при запуске cmake
message(STATUS "Fluxborn Build Info:")
message(STATUS "  OS:       ${FLUX_OS}")
message(STATUS "  Compiler: ${FLUX_COMPILER}")
message(STATUS "  Arch:     ${FLUX_ARCH}")
message(STATUS "  Output:   ${OUTPUT_BASE_DIR}")
