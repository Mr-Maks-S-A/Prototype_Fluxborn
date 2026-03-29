#pragma once
#include <map>
#include <tuple>
#include <memory>
#include "World/Chunk.hpp"








/**
 * @class World
 * @brief Управляет коллекцией чанков и их жизненным циклом.
 */
class World {
public:
    World(int seed); // Добавляем конструктор с сидом

    ~World() = default;



    // Метод для получения высоты в любой точке мира (нужен для коллизий)
    int GetHeightAt(int worldX, int worldZ);
    /**
     * @brief Генерирует начальную сетку чанков (например, 3x3).
     */
    void GenerateRegion(int viewDistance);

    /**
     * @brief Отрисовывает все активные чанки.
     * @param shader Указатель на шейдер для установки матрицы модели.
     */
    void Render(Shader* shader);

private:

    void GenerateChunkData(Chunk* chunk, int cx, int cz);


    FastNoiseLite m_Noise;
    int m_Seed;
    // Используем пару (x, z) как ключ для сетки чанков
    std::map<std::pair<int, int>, std::unique_ptr<Chunk>> m_Chunks;
};
