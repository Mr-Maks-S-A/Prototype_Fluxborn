#include "World/Chunk.hpp"


// Реализация конструктора
Chunk::Chunk() {
    // Инициализируем всё воздухом, чтобы не было мусора
    for (int x = 0; x < SIZE; x++)
        for (int y = 0; y < SIZE; y++)
            for (int z = 0; z < SIZE; z++)
                m_Blocks[x][y][z] = BlockType::Air;
}



void Chunk::LoadMesh(MeshData&& data) {
    // Если данных нет (пустой чанк), меш не создаем
    if (data.vertices.empty()) return;

    // Создаем Mesh (внутри glGenVertexArrays и т.д.)
    m_Mesh = std::make_unique<Mesh>(data.vertices, data.indices);
}



Chunk::MeshData Chunk::GenerateMeshData() {
    MeshData data;
    // Весь твой цикл for(x,y,z) из BuildMesh переезжает сюда
    // Вместо прямой вставки в m_Mesh, заполняем data.vertices и data.indices
    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            for (int z = 0; z < SIZE; z++) {
                BlockType type = m_Blocks[x][y][z];
                if (type == BlockType::Air) continue;

                float fx = (float)x; float fy = (float)y; float fz = (float)z;

                // Верх (Y+)
                if (IsTransparent(x, y + 1, z))
                    AddFace(data.vertices, data.indices, fx, fy, fz, FaceDirection::Top, type);

                // Низ (Y-)
                if (IsTransparent(x, y - 1, z))
                    AddFace(data.vertices, data.indices, fx, fy, fz, FaceDirection::Bottom, type);

                // Лево (X-)
                if (IsTransparent(x - 1, y, z))
                    AddFace(data.vertices, data.indices, fx, fy, fz, FaceDirection::Left, type);

                // Право (X+)
                if (IsTransparent(x + 1, y, z))
                    AddFace(data.vertices, data.indices, fx, fy, fz, FaceDirection::Right, type);

                // Перед (Z+)
                if (IsTransparent(x, y, z + 1))
                    AddFace(data.vertices, data.indices, fx, fy, fz, FaceDirection::Front, type);

                // Назад (Z-)
                if (IsTransparent(x, y, z - 1))
                    AddFace(data.vertices, data.indices, fx, fy, fz, FaceDirection::Back, type);

                // AddFace(data.vertices, data.indices, fx, fy, fz, FaceDirection::Top, type);
                // AddFace(data.vertices, data.indices, fx, fy, fz, FaceDirection::Bottom, type);
                // AddFace(data.vertices, data.indices, fx, fy, fz, FaceDirection::Left, type);
                // AddFace(data.vertices, data.indices, fx, fy, fz, FaceDirection::Right, type);
                // AddFace(data.vertices, data.indices, fx, fy, fz, FaceDirection::Front, type);
                // AddFace(data.vertices, data.indices, fx, fy, fz, FaceDirection::Back, type);
            }
        }
    }
    return data;
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


void Chunk::AddFace(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,
                    float x, float y, float z, FaceDirection dir, BlockType type) {

    uint32_t startIdx = static_cast<uint32_t>(vertices.size());

    // 1. Текстуры (оставляем твою логику)
    int texID = static_cast<int>(type) - 1;
    if (type == BlockType::Grass) {
        if (dir == FaceDirection::Top) texID = 0;
        else if (dir == FaceDirection::Bottom) texID = 2;
        else texID = 15;
    }

    const float step = 1.0f / 16.0f;
    float uMin = (texID % 16) * step;
    float vMin = 1.0f - ((texID / 16) + 1) * step;
    float uMax = uMin + step;
    float vMax = vMin + step;

    // 2. ГЕОМЕТРИЯ (Блок от x до x+1)
    float x0 = x; float x1 = x + 1.0f;
    float y0 = y; float y1 = y + 1.0f;
    float z0 = z; float z1 = z + 1.0f;

    switch (dir) {
        case FaceDirection::Top:
            vertices.push_back({ {x0, y1, z1}, {0, 1, 0}, {uMin, vMax} });
            vertices.push_back({ {x1, y1, z1}, {0, 1, 0}, {uMax, vMax} });
            vertices.push_back({ {x1, y1, z0}, {0, 1, 0}, {uMax, vMin} });
            vertices.push_back({ {x0, y1, z0}, {0, 1, 0}, {uMin, vMin} });
            break;
        case FaceDirection::Bottom:
            vertices.push_back({ {x0, y0, z0}, {0,-1, 0}, {uMin, vMin} });
            vertices.push_back({ {x1, y0, z0}, {0,-1, 0}, {uMax, vMin} });
            vertices.push_back({ {x1, y0, z1}, {0,-1, 0}, {uMax, vMax} });
            vertices.push_back({ {x0, y0, z1}, {0,-1, 0}, {uMin, vMax} });
            break;
        case FaceDirection::Left: // X-
            vertices.push_back({ {x0, y0, z0}, {-1, 0, 0}, {uMin, vMin} });
            vertices.push_back({ {x0, y0, z1}, {-1, 0, 0}, {uMax, vMin} });
            vertices.push_back({ {x0, y1, z1}, {-1, 0, 0}, {uMax, vMax} });
            vertices.push_back({ {x0, y1, z0}, {-1, 0, 0}, {uMin, vMax} });
            break;
        case FaceDirection::Right: // X+
            vertices.push_back({ {x1, y0, z1}, {1, 0, 0}, {uMin, vMin} });
            vertices.push_back({ {x1, y0, z0}, {1, 0, 0}, {uMax, vMin} });
            vertices.push_back({ {x1, y1, z0}, {1, 0, 0}, {uMax, vMax} });
            vertices.push_back({ {x1, y1, z1}, {1, 0, 0}, {uMin, vMax} });
            break;
        case FaceDirection::Front: // Z+
            vertices.push_back({ {x0, y0, z1}, {0, 0, 1}, {uMin, vMin} });
            vertices.push_back({ {x1, y0, z1}, {0, 0, 1}, {uMax, vMin} });
            vertices.push_back({ {x1, y1, z1}, {0, 0, 1}, {uMax, vMax} });
            vertices.push_back({ {x0, y1, z1}, {0, 0, 1}, {uMin, vMax} });
            break;
        case FaceDirection::Back: // Z-
            vertices.push_back({ {x1, y0, z0}, {0, 0,-1}, {uMin, vMin} });
            vertices.push_back({ {x0, y0, z0}, {0, 0,-1}, {uMax, vMin} });
            vertices.push_back({ {x0, y1, z0}, {0, 0,-1}, {uMax, vMax} });
            vertices.push_back({ {x1, y1, z0}, {0, 0,-1}, {uMin, vMax} });
            break;
    }

    // 3. Индексы (против часовой стрелки для Cull Face)
    indices.push_back(startIdx + 0); indices.push_back(startIdx + 1); indices.push_back(startIdx + 2);
    indices.push_back(startIdx + 2); indices.push_back(startIdx + 3); indices.push_back(startIdx + 0);
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

                BlockType type = m_Blocks[x][y][z]; // 1. Получаем тип текущего блока
                if (type == BlockType::Air) continue;

                float fx = (float)x;
                float fy = (float)y;
                float fz = (float)z;

                // 2. Передаем 'type' последним аргументом в каждый вызов AddFace

                // Верх (Y+)
                if (IsTransparent(x, y + 1, z))
                    AddFace(vertices, indices, fx, fy, fz, FaceDirection::Top, type);

                // Низ (Y-)
                if (IsTransparent(x, y - 1, z))
                    AddFace(vertices, indices, fx, fy, fz, FaceDirection::Bottom, type);

                // Лево (X-)
                if (IsTransparent(x - 1, y, z))
                    AddFace(vertices, indices, fx, fy, fz, FaceDirection::Left, type);

                // Право (X+)
                if (IsTransparent(x + 1, y, z))
                    AddFace(vertices, indices, fx, fy, fz, FaceDirection::Right, type);

                // Перед (Z+)
                if (IsTransparent(x, y, z + 1))
                    AddFace(vertices, indices, fx, fy, fz, FaceDirection::Front, type);

                // Зад (Z-)
                if (IsTransparent(x, y, z - 1))
                    AddFace(vertices, indices, fx, fy, fz, FaceDirection::Back, type);
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
