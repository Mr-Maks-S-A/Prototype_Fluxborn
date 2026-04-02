/**
 * @file Chunk.hpp
 * @brief Класс управления воксельным чанком.
 */

#pragma once
#include "Renderer/Mesh.hpp"
#include "World/BlockType.hpp"
#include "Renderer/Shader.hpp" // Теперь подключаем здесь!
#include <vector>
#include <memory>
#include "FastNoiseLite.h"

/**
 * @enum FaceDirection
 * @brief Направления граней блока для генерации геометрии.
 */
enum class FaceDirection {
    Top,    ///< Верхняя грань (Y+)
    Bottom, ///< Нижняя грань (Y-)
    Left,   ///< Левая грань (X-)
    Right,  ///< Правая грань (X+)
    Front,  ///< Передняя грань (Z+)
    Back    ///< Задняя грань (Z-)
};

/**
 * @class Chunk
 * @brief Представляет собой фиксированный объем воксельного мира (32x32x32).
 * * Отвечает за:
 * 1. Хранение данных о блоках в трехмерном массиве.
 * 2. Генерацию оптимизированной геометрии (Culled Mesh).
 * 3. Расчет текстурных координат на основе атласа.
 */
class Chunk {
public:

    /**
     * @struct MeshData
     * @brief Промежуточный контейнер для данных меша перед отправкой в GPU.
     */
    struct MeshData {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

    static const int SIZE = 32; ///< Сторона куба чанка. Всего 32,768 блоков на чанк.

    Chunk();


    /**
     * @brief Генерирует геометрию чанка в ОЗУ.
     * @return MeshData Набор вершин и индексов. Можно вызывать в фоновом потоке.
     */
    MeshData GenerateMeshData();


    /**
     * @brief Загружает готовую геометрию в GPU.
     * @param data Данные, полученные из GenerateMeshData.
     * @note Вызывать ТОЛЬКО в основном потоке.
     */
    void LoadMesh(MeshData&& data);

    /**
     * @brief Жадная генерация меша.
     * * Проходит по каждому блоку и проверяет 6 соседних позиций через IsTransparent().
     * Грань создается только если соседний блок прозрачен.
     */
    void BuildMesh();

    /**
     * @brief Отрисовка чанка.
     * * Автоматически устанавливает матрицу u_Model на основе мировых координат.
     */
    void Render(Shader* shader);

    // Вспомогательные методы для World
    void SetBlock(int x, int y, int z, BlockType type) { m_Blocks[x][y][z] = type; }
    BlockType GetBlock(int x, int y, int z) const { return m_Blocks[x][y][z]; }


    void SetWorldPos(const glm::ivec3& pos) { m_WorldPos = pos; }
    glm::ivec3 GetPosition() const { return m_WorldPos; }
private:


    /**
     * @brief Проверяет, является ли блок по указанным координатам прозрачным.
     * @param x Локальная координата X.
     * @param y Локальная координата Y.
     * @param z Локальная координата Z.
     * @return true, если блок прозрачен или находится за пределами чанка.
     */
    bool IsTransparent(int x, int y, int z) const;

    /**
     * @brief Добавляет 4 вершины и 6 индексов (2 треугольника) для грани блока.
     * * Вычисляет UV-координаты из атласа 16x16 на основе типа блока и направления грани.
     * @param vertices Вектор вершин для заполнения.
     * @param indices Вектор индексов для заполнения.
     * @param x,y,z Мировые координаты центра блока.
     * @param dir Направление отрисовываемой грани.
     * @param type Тип блока (нужен для выбора текстуры).
     */
    void AddFace(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,
                 float x, float y, float z, FaceDirection dir, BlockType type);

    BlockType m_Blocks[SIZE][SIZE][SIZE]; ///< Массив данных о блоках чанка.
    std::unique_ptr<Mesh> m_Mesh;         ///< Сгенерированная полигональная сетка чанка.
    glm::ivec3 m_WorldPos = {0, 0, 0};    // Позиция чанка в мировых координатах (в сетке чанков)
};
