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


void Chunk::AddFace(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,
                    float x, float y, float z, FaceDirection dir, BlockType type) {

    uint32_t startIdx = static_cast<uint32_t>(vertices.size());

    // 1. Опеределяем индекс текстуры в атласе
    int texID = static_cast<int>(type) - 1; // Grass(1) станет 0, Dirt(2) станет 1 и т.д.

    // ЛОГИКА ДЛЯ РАЗНЫХ СТОРОН (Пример для Травы)
    if (type == BlockType::Grass) {
        if (dir == FaceDirection::Top) texID = 0;    // Чистая трава
        else if (dir == FaceDirection::Bottom) texID = 2; // Земля
        else texID = 15; // Бок травы (если он есть в атласе на 16-й позиции)
    }

    // 2. Расчет координат в атласе 16x16
    const float step = 1.0f / 16.0f;
    float uMin = (texID % 16) * step;
    // float vMin = (texID / 16) * step;

    // ВАЖНО: Если текстуры в атласе перевернуты, используй:
    float vMin = 1.0f - ((texID / 16) + 1) * step;

    float uMax = uMin + step;
    float vMax = vMin + step;

    // Вспомогательные смещения (без изменений)
    float x_p = x + 0.5f; float x_m = x - 0.5f;
    float y_p = y + 0.5f; float y_m = y - 0.5f;
    float z_p = z + 0.5f; float z_m = z - 0.5f;


    switch (dir) {
        case FaceDirection::Top:
            vertices.push_back({ {x_m, y_p, z_p}, {0, 1, 0}, {uMin, vMax} });
            vertices.push_back({ {x_p, y_p, z_p}, {0, 1, 0}, {uMax, vMax} });
            vertices.push_back({ {x_p, y_p, z_m}, {0, 1, 0}, {uMax, vMin} });
            vertices.push_back({ {x_m, y_p, z_m}, {0, 1, 0}, {uMin, vMin} });
            break;
        case FaceDirection::Bottom:
            vertices.push_back({ {x_m, y_m, z_m}, {0,-1, 0}, {uMin, vMin} });
            vertices.push_back({ {x_p, y_m, z_m}, {0,-1, 0}, {uMax, vMin} });
            vertices.push_back({ {x_p, y_m, z_p}, {0,-1, 0}, {uMax, vMax} });
            vertices.push_back({ {x_m, y_m, z_p}, {0,-1, 0}, {uMin, vMax} });
            break;
        case FaceDirection::Left:
            vertices.push_back({ {x_m, y_m, z_m}, {-1, 0, 0}, {uMin, vMin} });
            vertices.push_back({ {x_m, y_m, z_p}, {-1, 0, 0}, {uMax, vMin} });
            vertices.push_back({ {x_m, y_p, z_p}, {-1, 0, 0}, {uMax, vMax} });
            vertices.push_back({ {x_m, y_p, z_m}, {-1, 0, 0}, {uMin, vMax} });
            break;
        case FaceDirection::Right:
            vertices.push_back({ {x_p, y_m, z_p}, {1, 0, 0}, {uMin, vMin} });
            vertices.push_back({ {x_p, y_m, z_m}, {1, 0, 0}, {uMax, vMin} });
            vertices.push_back({ {x_p, y_p, z_m}, {1, 0, 0}, {uMax, vMax} });
            vertices.push_back({ {x_p, y_p, z_p}, {1, 0, 0}, {uMin, vMax} });
            break;
        case FaceDirection::Front:
            vertices.push_back({ {x_m, y_m, z_p}, {0, 0, 1}, {uMin, vMin} });
            vertices.push_back({ {x_p, y_m, z_p}, {0, 0, 1}, {uMax, vMin} });
            vertices.push_back({ {x_p, y_p, z_p}, {0, 0, 1}, {uMax, vMax} });
            vertices.push_back({ {x_m, y_p, z_p}, {0, 0, 1}, {uMin, vMax} });
            break;
        case FaceDirection::Back:
            vertices.push_back({ {x_p, y_m, z_m}, {0, 0,-1}, {uMin, vMin} });
            vertices.push_back({ {x_m, y_m, z_m}, {0, 0,-1}, {uMax, vMin} });
            vertices.push_back({ {x_m, y_p, z_m}, {0, 0,-1}, {uMax, vMax} });
            vertices.push_back({ {x_p, y_p, z_m}, {0, 0,-1}, {uMin, vMax} });
            break;
    }

    // Индексы для двух треугольников грани
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
