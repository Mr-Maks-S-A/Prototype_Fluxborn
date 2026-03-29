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

void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) {
    uint32_t bindingIndex = m_VertexBufferIndex++;

    // Шаг (stride) теперь 6 флоатов (3 pos + 3 normal)
    glVertexArrayVertexBuffer(m_RendererID, bindingIndex, vertexBuffer->m_RendererID, 0, sizeof(Vertex));

    // Атрибут 0: Позиция
    glEnableVertexArrayAttrib(m_RendererID, 0);
    glVertexArrayAttribFormat(m_RendererID, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Position));
    glVertexArrayAttribBinding(m_RendererID, 0, bindingIndex);

    // Атрибут 1: Нормаль
    glEnableVertexArrayAttrib(m_RendererID, 1);
    glVertexArrayAttribFormat(m_RendererID, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Normal));
    glVertexArrayAttribBinding(m_RendererID, 1, bindingIndex);

    m_VertexBuffers.push_back(vertexBuffer);
}

void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) {
    glVertexArrayElementBuffer(m_RendererID, indexBuffer->m_RendererID);
    m_IndexBuffer = indexBuffer;
}
