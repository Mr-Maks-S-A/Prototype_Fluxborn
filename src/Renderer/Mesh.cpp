#include "Renderer/Mesh.hpp"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
    m_VAO = std::make_shared<VertexArray>();

    // Загружаем вершины
    m_VBO = std::make_shared<VertexBuffer>(vertices.data(), vertices.size() * sizeof(Vertex));

    // Добавляем буфер в VAO
    m_VAO->AddVertexBuffer(m_VBO);

    // Устанавливаем индексы
    m_EBO = std::make_shared<IndexBuffer>(indices.data(), indices.size());
    m_VAO->SetIndexBuffer(m_EBO);
}

void Mesh::Draw() const {
    m_VAO->Bind();
    glDrawElements(GL_TRIANGLES, m_VAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
    m_VAO->Unbind();
}
