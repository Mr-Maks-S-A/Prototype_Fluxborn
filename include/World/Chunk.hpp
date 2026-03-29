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
 * @brief Представляет собой фиксированный объем воксельного мира (16x16x16).
 * * Класс отвечает за хранение данных о блоках и эффективную генерацию
 * полигональной сетки (Mesh) на основе видимости граней.
 */
class Chunk {
public:
    static const int SIZE = 32; ///< Размер стороны чанка в блоках.

    Chunk();

    /**
     * @brief Генерирует Mesh чанка, отсекая невидимые грани.
     * * Алгоритм проходит по всем блокам и добавляет грани только в тех
     * направлениях, где соседний блок является прозрачным (воздухом).
     */
    void BuildMesh();

    /**
     * @brief Отрисовывает сгенерированный Mesh чанка.
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
     * @brief Добавляет вершины и индексы конкретной грани блока в буферы.
     * @param vertices Вектор вершин для заполнения.
     * @param indices Вектор индексов для заполнения.
     * @param x,y,z Мировые координаты центра блока.
     * @param dir Направление отрисовываемой грани.
     */
    void AddFace(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,
                 float x, float y, float z, FaceDirection dir, BlockType type);

    BlockType m_Blocks[SIZE][SIZE][SIZE]; ///< Массив данных о блоках чанка.
    std::unique_ptr<Mesh> m_Mesh;         ///< Сгенерированная полигональная сетка чанка.
    glm::ivec3 m_WorldPos = {0, 0, 0};    // Позиция чанка в мировых координатах (в сетке чанков)
};
