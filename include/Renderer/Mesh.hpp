/**
 * @file Mesh.hpp
 * @brief Класс для управления низкоуровневыми геометрическими данными.
 */

#pragma once

#include "Renderer/VertexArray.hpp"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

/**
 * @struct Vertex
 * @brief Полное описание вершины для 3D-пространства.
 * * Память выровнена для прямой передачи в GPU буфер.
 */
struct Vertex {
    glm::vec3 Position;  ///< Позиция (x, y, z).
    glm::vec3 Normal;    ///< Нормаль для расчета освещения.
    glm::vec2 texCoords; ///< UV-координаты для наложения текстур.
};

/**
 * @class Mesh
 * @brief Обертка над объектами буферов OpenGL (VAO, VBO, EBO).
 * * Автоматизирует процесс привязки данных к конвейеру отрисовки.
 * * Поддерживает индексированную отрисовку для экономии видеопамяти.
 */
class Mesh {
public:
    /**
     * @brief Конструктор меша.
     * * Создает Vertex Array Object и наполняет его данными вершин и индексов.
     * @param vertices Вектор структур Vertex.
     * @param indices Вектор индексов для формирования треугольников.
     */
    Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

    ~Mesh() = default;

    /**
     * @brief Выполняет отрисовку объекта.
     * * Вызывает glDrawElements для эффективного рендеринга геометрии.
     */
    void Draw() const;

private:
    std::shared_ptr<VertexArray>  m_VAO; ///< Хранит состояние связей атрибутов.
    std::shared_ptr<VertexBuffer> m_VBO; ///< Сырые данные вершин.
    std::shared_ptr<IndexBuffer>  m_EBO; ///< Порядок отрисовки вершин.
};
