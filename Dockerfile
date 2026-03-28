# --- ЭТАП 1: Сборка под Linux (Wayland + X11) ---
FROM archlinux:latest AS build-linux
RUN pacman -Syu --noconfirm && \
    pacman -S --noconfirm cmake ninja gcc git wayland wayland-protocols \
    libxkbcommon libx11 libxrandr libxinerama libxcursor libxi mesa

WORKDIR /app
COPY . .
# Собираем Linux версию
RUN cmake -B build-linux -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DGLFW_BUILD_WAYLAND=ON \
    -DGLFW_BUILD_X11=ON
RUN cmake --build build-linux

# --- ЭТАП 2: Сборка под Windows (MinGW) ---
FROM archlinux:latest AS build-windows
RUN pacman -Syu --noconfirm && \
    pacman -S --noconfirm cmake ninja mingw-w64-gcc git

WORKDIR /app
COPY . .
# Собираем Windows версию через тулчейн
RUN cmake -B build-win -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE=Cmake/toolchains/windows-mingw.cmake \
    -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build-win

# --- ЭТАП 3: Экспорт ---
FROM scratch AS export
# Копируем результаты из папок, которые определены в твоем CMakeLists.txt
COPY --from=build-linux /app/bin/Linux /Linux
COPY --from=build-windows /app/bin/Windows /Windows
