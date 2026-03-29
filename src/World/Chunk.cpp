#include "World/Chunk.hpp"


// Реализация конструктора
Chunk::Chunk() {
    // Инициализируем всё воздухом, чтобы не было мусора
    for (int x = 0; x < SIZE; x++)
        for (int y = 0; y < SIZE; y++)
            for (int z = 0; z < SIZE; z++)
                m_Blocks[x][y][z] = BlockType::Air;
}

/**
 * @brief Отрисовывает чанк в мировых координатах.
 * @param shader Указатель на активную шейдерную программу.
 * * Метод вычисляет матрицу модели на основе мирового положения чанка
 * (m_WorldPos * SIZE) и передает её в шейдер как uniform u_Model.
 */
void Chunk::Render(Shader* shader) {
    if (!m_Mesh) return;

    // Вычисляем позицию чанка в мировом пространстве
    // m_WorldPos — это координаты в сетке (0, 1, 2...), умножаем на размер чанка
    glm::vec3 worldPos = glm::vec3(
        m_WorldPos.x * SIZE,
        m_WorldPos.y * SIZE,
        m_WorldPos.z * SIZE
    );

    // Создаем матрицу трансформации
    glm::mat4 model = glm::translate(glm::mat4(1.0f), worldPos);

    // Загружаем её в шейдер
    shader->SetMat4("u_Model", model);

    // Рисуем
    m_Mesh->Draw();
}


void Chunk::AddFace(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, float x, float y, float z, FaceDirection dir) {
    uint32_t startIdx = (uint32_t)vertices.size();

    switch(dir) {
        case FaceDirection::Top:
            vertices.push_back({{x - 0.5f, y + 0.5f, z + 0.5f}, {0, 1, 0}});
            vertices.push_back({{x + 0.5f, y + 0.5f, z + 0.5f}, {0, 1, 0}});
            vertices.push_back({{x + 0.5f, y + 0.5f, z - 0.5f}, {0, 1, 0}});
            vertices.push_back({{x - 0.5f, y + 0.5f, z - 0.5f}, {0, 1, 0}});
            break;
        case FaceDirection::Bottom:
            vertices.push_back({{x - 0.5f, y - 0.5f, z - 0.5f}, {0, -1, 0}});
            vertices.push_back({{x + 0.5f, y - 0.5f, z - 0.5f}, {0, -1, 0}});
            vertices.push_back({{x + 0.5f, y - 0.5f, z + 0.5f}, {0, -1, 0}});
            vertices.push_back({{x - 0.5f, y - 0.5f, z + 0.5f}, {0, -1, 0}});
            break;
        case FaceDirection::Left:
            vertices.push_back({{x - 0.5f, y + 0.5f, z + 0.5f}, {-1, 0, 0}});
            vertices.push_back({{x - 0.5f, y + 0.5f, z - 0.5f}, {-1, 0, 0}});
            vertices.push_back({{x - 0.5f, y - 0.5f, z - 0.5f}, {-1, 0, 0}});
            vertices.push_back({{x - 0.5f, y - 0.5f, z + 0.5f}, {-1, 0, 0}});
            break;
        case FaceDirection::Right:
            vertices.push_back({{x + 0.5f, y + 0.5f, z - 0.5f}, {1, 0, 0}});
            vertices.push_back({{x + 0.5f, y + 0.5f, z + 0.5f}, {1, 0, 0}});
            vertices.push_back({{x + 0.5f, y - 0.5f, z + 0.5f}, {1, 0, 0}});
            vertices.push_back({{x + 0.5f, y - 0.5f, z - 0.5f}, {1, 0, 0}});
            break;
        case FaceDirection::Front:
            vertices.push_back({{x - 0.5f, y + 0.5f, z + 0.5f}, {0, 0, 1}});
            vertices.push_back({{x - 0.5f, y - 0.5f, z + 0.5f}, {0, 0, 1}});
            vertices.push_back({{x + 0.5f, y - 0.5f, z + 0.5f}, {0, 0, 1}});
            vertices.push_back({{x + 0.5f, y + 0.5f, z + 0.5f}, {0, 0, 1}});
            break;
        case FaceDirection::Back:
            vertices.push_back({{x + 0.5f, y + 0.5f, z - 0.5f}, {0, 0, -1}});
            vertices.push_back({{x + 0.5f, y - 0.5f, z - 0.5f}, {0, 0, -1}});
            vertices.push_back({{x - 0.5f, y - 0.5f, z - 0.5f}, {0, 0, -1}});
            vertices.push_back({{x - 0.5f, y + 0.5f, z - 0.5f}, {0, 0, -1}});
            break;
    }

    // Индексы одинаковы для любой грани (два треугольника)
    indices.push_back(startIdx + 0); indices.push_back(startIdx + 1); indices.push_back(startIdx + 2);
    indices.push_back(startIdx + 0); indices.push_back(startIdx + 2); indices.push_back(startIdx + 3);
}


bool Chunk::IsTransparent(int x, int y, int z) const {
    // Если координата за пределами текущего чанка,
    // пока считаем, что там воздух (позже заменим на проверку соседних чанков)
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE || z < 0 || z >= SIZE)
        return true;

    return m_Blocks[x][y][z] == BlockType::Air;
}

void Chunk::BuildMesh() {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            for (int z = 0; z < SIZE; z++) {

                if (m_Blocks[x][y][z] == BlockType::Air) continue;

                float fx = (float)x;
                float fy = (float)y;
                float fz = (float)z;

                // Верх (Y+)
                if (IsTransparent(x, y + 1, z))
                    AddFace(vertices, indices, fx, fy, fz, FaceDirection::Top);

                // Низ (Y-)
                if (IsTransparent(x, y - 1, z))
                    AddFace(vertices, indices, fx, fy, fz, FaceDirection::Bottom);

                // Лево (X-)
                if (IsTransparent(x - 1, y, z))
                    AddFace(vertices, indices, fx, fy, fz, FaceDirection::Left);

                // Право (X+)
                if (IsTransparent(x + 1, y, z))
                    AddFace(vertices, indices, fx, fy, fz, FaceDirection::Right);

                // Перед (Z+)
                if (IsTransparent(x, y, z + 1))
                    AddFace(vertices, indices, fx, fy, fz, FaceDirection::Front);

                // Зад (Z-)
                if (IsTransparent(x, y, z - 1))
                    AddFace(vertices, indices, fx, fy, fz, FaceDirection::Back);
            }
        }
    }
    m_Mesh = std::make_unique<Mesh>(vertices, indices);
}


/**
 * @brief Тестовое заполнение чанка блоками.
 * Создает простую волнистую поверхность для проверки рендеринга и стыковки.
 */
/*void Chunk::SetupWorld() {
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.05f); // Настрой под нужную крутизну холмов

    for (int x = 0; x < SIZE; x++) {
        for (int z = 0; z < SIZE; z++) {
            float globalX = (float)(m_WorldPos.x * SIZE + x);
            float globalZ = (float)(m_WorldPos.z * SIZE + z);

            // Получаем шум (-1...1) и переводим в высоту
            float n = noise.GetNoise(globalX, globalZ);
            int height = static_cast<int>((n + 1.0f) * 0.5f * SIZE);

            for (int y = 0; y < SIZE; y++) {
                if (y < height - 1) {
                    m_Blocks[x][y][z] = BlockType::Dirt; // Глубже — земля
                } else if (y == height - 1) {
                    m_Blocks[x][y][z] = BlockType::Grass; // Сверху — трава
                } else {
                    m_Blocks[x][y][z] = BlockType::Air;
                }
            }
        }
    }
    BuildMesh();
 }
 */
