/**
 * @file VertexArray.hpp
 * @brief Класс управления Vertex Array Objects (VAO).
 */

#pragma once

#include "Renderer/Buffer.hpp"
#include <memory>
#include <vector>

/**
 * @class VertexArray
 * @brief Инкапсулирует состояние атрибутов вершин.
 * Связывает буферы данных с логикой шейдера.
 */
class VertexArray {
public:
    VertexArray();
    ~VertexArray();

    void Bind() const;
    void Unbind() const;

    /**
     * @brief Добавляет буфер вершин и настраивает его разметку (layout).
     * Для простоты пока захардкодим разметку (Pos: 3 float).
     */
    void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);

    /**
     * @brief Устанавливает индексный буфер.
     */
    void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

    const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }

private:
    uint32_t m_RendererID;
    uint32_t m_VertexBufferIndex = 0;
    std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
    std::shared_ptr<IndexBuffer> m_IndexBuffer;
};
