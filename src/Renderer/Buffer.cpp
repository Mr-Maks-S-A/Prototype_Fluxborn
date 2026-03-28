#include "Renderer/Buffer.hpp"

// --- VertexBuffer ---

VertexBuffer::VertexBuffer(const void* vertices, uint32_t size) {
    glCreateBuffers(1, &m_RendererID);
    // Используем GL_DYNAMIC_STORAGE_BIT, так как чанки могут перестраиваться
    glNamedBufferStorage(m_RendererID, size, vertices, GL_DYNAMIC_STORAGE_BIT);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &m_RendererID);
}

void VertexBuffer::Bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_RendererID); }
void VertexBuffer::Unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

// --- IndexBuffer ---

IndexBuffer::IndexBuffer(const uint32_t* indices, uint32_t count) : m_Count(count) {
    glCreateBuffers(1, &m_RendererID);
    glNamedBufferStorage(m_RendererID, count * sizeof(uint32_t), indices, GL_DYNAMIC_STORAGE_BIT);
}

IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &m_RendererID);
}

void IndexBuffer::Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID); }
void IndexBuffer::Unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
