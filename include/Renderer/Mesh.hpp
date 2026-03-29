/**
 * @file Mesh.hpp
 * @brief Класс для управления геометрическими объектами.
 */

#pragma once

#include "Renderer/VertexArray.hpp"
#include <memory>
#include <vector>
#include <glm/glm.hpp>


/**
 * @struct Vertex
 * @brief Структура вершины (Позиция).
 */
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
};

class Mesh {
public:
    /**
     * @brief Создает меш из набора вершин и индексов.
     */
    Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    ~Mesh() = default;

    /**
     * @brief Отрисовка меша.
     */
    void Draw() const;

private:
    std::shared_ptr<VertexArray>  m_VAO;
    std::shared_ptr<VertexBuffer> m_VBO;
    std::shared_ptr<IndexBuffer>  m_EBO;
};
