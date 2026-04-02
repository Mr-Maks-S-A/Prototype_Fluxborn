#include "Renderer/VertexArray.hpp"
#include "Renderer/Mesh.hpp"


VertexArray::VertexArray() {
    glCreateVertexArrays(1, &m_RendererID);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::Bind() const { glBindVertexArray(m_RendererID); }
void VertexArray::Unbind() const { glBindVertexArray(0); }

// void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) {
//     uint32_t bindingIndex = m_VertexBufferIndex++;
//
//     // Привязываем VBO к VAO (stride берется из sizeof(Vertex), что уже включает Position, Normal и texCoords)
//     glVertexArrayVertexBuffer(m_RendererID, bindingIndex, vertexBuffer->m_RendererID, 0, sizeof(Vertex));
//
//     // Атрибут 0: Позиция (vec3)
//     glEnableVertexArrayAttrib(m_RendererID, 0);
//     glVertexArrayAttribFormat(m_RendererID, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Position));
//     glVertexArrayAttribBinding(m_RendererID, 0, bindingIndex);
//
//     // Атрибут 1: Нормаль (vec3)
//     glEnableVertexArrayAttrib(m_RendererID, 1);
//     glVertexArrayAttribFormat(m_RendererID, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Normal));
//     glVertexArrayAttribBinding(m_RendererID, 1, bindingIndex);
//
//     // НОВОЕ: Атрибут 2: Текстурные координаты (vec2)
//     glEnableVertexArrayAttrib(m_RendererID, 2);
//     glVertexArrayAttribFormat(m_RendererID, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoords));
//     glVertexArrayAttribBinding(m_RendererID, 2, bindingIndex);
//
//     m_VertexBuffers.push_back(vertexBuffer);
// }

void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) {
    // Используем фиксированный индекс 0, так как у нас один меш — один буфер
    const uint32_t bindingIndex = 0;

    // Привязываем VBO
    glVertexArrayVertexBuffer(m_RendererID, bindingIndex, vertexBuffer->GetRendererID(), 0, sizeof(Vertex));

    // Атрибут 0: Позиция
    glEnableVertexArrayAttrib(m_RendererID, 0);
    glVertexArrayAttribFormat(m_RendererID, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Position));
    glVertexArrayAttribBinding(m_RendererID, 0, bindingIndex);

    // Атрибут 1: Нормаль
    glEnableVertexArrayAttrib(m_RendererID, 1);
    glVertexArrayAttribFormat(m_RendererID, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Normal));
    glVertexArrayAttribBinding(m_RendererID, 1, bindingIndex);

    // Атрибут 2: UV
    glEnableVertexArrayAttrib(m_RendererID, 2);
    glVertexArrayAttribFormat(m_RendererID, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoords));
    glVertexArrayAttribBinding(m_RendererID, 2, bindingIndex);

    m_VertexBuffers.push_back(vertexBuffer);
}


void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) {
    glVertexArrayElementBuffer(m_RendererID, indexBuffer->m_RendererID);
    m_IndexBuffer = indexBuffer;
}
