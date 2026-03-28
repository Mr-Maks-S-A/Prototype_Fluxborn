/**
 * @file Buffer.hpp
 * @brief Классы управления буферами данных OpenGL.
 */

#pragma once

#include <glad/glad.h>
#include <vector>


// Опережающее объявление, чтобы классы буферов знали, кто такой VertexArray
class VertexArray;


/**
 * @class VertexBuffer
 * @brief Объект буфера вершин (VBO). Хранит сырые данные о точках, цветах и т.д.
 */
class VertexBuffer {
public:
    friend class VertexArray;
    /**
     * @brief Создает VBO и загружает в него данные.
     * @param vertices Указатель на массив данных.
     * @param size Размер данных в байтах.
     */
    VertexBuffer(const void* vertices, uint32_t size);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;

private:
    uint32_t m_RendererID;
};

/**
 * @class IndexBuffer
 * @brief Объект буфера индексов (EBO). Позволяет переиспользовать вершины.
 * Критически важен для вокселей (чтобы не дублировать вершины на стыках кубов).
 */
class IndexBuffer {
public:
    friend class VertexArray;
    IndexBuffer(const uint32_t* indices, uint32_t count);
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    inline uint32_t GetCount() const { return m_Count; }

private:
    uint32_t m_RendererID;
    uint32_t m_Count;
};
