cmake_minimum_required(VERSION 4.0 FATAL_ERROR)
project(Inspector VERSION 1.0.0)

# --- ГЛОБАЛЬНЫЕ НАСТРОЙКИ ---
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

# --- ВЫХОДНЫЕ КАТАЛОГИ ---
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# --- ИСХОДНЫЕ ФАЙЛЫ ---
SET(LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/Inspector.cpp")

SET(LIB_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/Until/Types.hpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/Until/Inspector.hpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/Until/Defines.hpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/Until/Registry.hpp") # Поправил маску

# --- БИБЛИОТЕКА ---
add_library(Inspector STATIC ${LIB_SOURCES} ${LIB_HEADERS})

# Алиас для удобства (чтобы внутри проекта писать Until::Inspector)
add_library(Until::Inspector ALIAS Inspector)

#Используем абсолютный путь через ${CMAKE_CURRENT_SOURCE_DIR}
target_include_directories(Inspector
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(Inspector PROPERTIES LINKER_LANGUAGE CXX)

# Настройки компилятора
if(MSVC)
    target_compile_options(Inspector PRIVATE /W4)
else()
    target_compile_options(Inspector PRIVATE -Wall -Wextra -Wpedantic)
endif()

# --- ПРИМЕРЫ И ТЕСТЫ (Опционально) ---
option(UNTIL_BUILD_EXAMPLES "Build example applications" ON)
option(UNTIL_BUILD_TESTS "Build unit tests" OFF)

if(UNTIL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(UNTIL_BUILD_TESTS)
    add_subdirectory(tests)
endif()
