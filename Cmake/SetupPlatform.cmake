# ==============================================================================
# PROJECT: Fluxborn Engine
# MODULE:  Platform & Compiler Setup (Strict Hierarchy v2.0)
# ==============================================================================

# --- 1. ОПРЕДЕЛЕНИЕ ТИПА СБОРКИ (BUILD TYPE) ---
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Поток для формирования пути: если CMAKE_BUILD_TYPE пуст (в VS), берем $<CONFIG>
if(CMAKE_BUILD_TYPE)
    set(FLUX_BUILD_CONFIG ${CMAKE_BUILD_TYPE})
else()
    set(FLUX_BUILD_CONFIG "$<CONFIG>")
endif()

# --- 2. ОПРЕДЕЛЕНИЕ ОС (OS) ---
if(WIN32)
    set(FLUX_OS "Windows")
elseif(APPLE)
    set(FLUX_OS "macOS")
elseif(UNIX)
    if(EXISTS "/etc/os-release")
        file(STRINGS "/etc/os-release" OS_ID_LINE REGEX "^ID=")
        string(REGEX REPLACE "ID=(.*)" "\\1" FLUX_DISTRO "${OS_ID_LINE}")
        string(REPLACE "\"" "" FLUX_DISTRO "${FLUX_DISTRO}")
        string(REPLACE " " "_" FLUX_DISTRO "${FLUX_DISTRO}")
        set(FLUX_OS "Linux_${FLUX_DISTRO}")
    else()
        set(FLUX_OS "Linux_Generic")
    endif()
endif()

# --- 3. АРХИТЕКТУРА (ARCH) ---
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "(arm64|aarch64)")
        set(FLUX_ARCH "ARM64")
    else()
        set(FLUX_ARCH "x64")
    endif()
else()
    set(FLUX_ARCH "x32")
endif()

# --- 4. КОМПИЛЯТОР (COMPILER) ---
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(FLUX_COMPILER "GCC")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(FLUX_COMPILER "Clang")
elseif(MSVC)
    set(FLUX_COMPILER "MSVC")
else()
    set(FLUX_COMPILER ${CMAKE_CXX_COMPILER_ID})
endif()
set(FLUX_COMPILER "${FLUX_COMPILER}_${CMAKE_CXX_COMPILER_VERSION_MAJOR}")

# --- 5. УПРАВЛЕНИЕ ВЫХОДНЫМИ ДИРЕКТОРИЯМИ ---
# Структура: build/{bin|lib}/{OS}/{ARCH}/{COMPILER}/{CONFIG}

# Базовая часть пути
set(FLUX_PATH_SUFFIX "${FLUX_OS}/${FLUX_ARCH}/${FLUX_COMPILER}/${FLUX_BUILD_CONFIG}")

# Путь для исполняемых файлов (Runtime)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/bin/${FLUX_PATH_SUFFIX}")

# Пути для библиотек (Static/Shared)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/lib/${FLUX_PATH_SUFFIX}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/lib/${FLUX_PATH_SUFFIX}")

# --- 6. ИНФОРМАЦИОННЫЙ ВЫВОД ---
message(STATUS "-------------------------------------------------------")
message(STATUS "Fluxborn Environment:")
message(STATUS "  > Config:     ${FLUX_BUILD_CONFIG}")
message(STATUS "  > Platform:   ${FLUX_OS} (${FLUX_ARCH})")
message(STATUS "  > Compiler:   ${FLUX_COMPILER}")
message(STATUS "  > Bin Path:   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "-------------------------------------------------------")
