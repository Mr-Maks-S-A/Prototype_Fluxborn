#include "World/World.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

World::World(int seed) : m_Seed(seed) {
    m_Noise.SetSeed(m_Seed);
    m_Noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_Noise.SetFrequency(0.01f); // Настройка "плавности" ландшафта
}



int World::GetHeightAt(int worldX, int worldZ) {
    // 1. Находим, в каком чанке находятся координаты
    int cx = std::floor((float)worldX / Chunk::SIZE);
    int cz = std::floor((float)worldZ / Chunk::SIZE);

    // 2. Локальные координаты внутри чанка
    int lx = worldX % Chunk::SIZE;
    int lz = worldZ % Chunk::SIZE;
    if (lx < 0) lx += Chunk::SIZE;
    if (lz < 0) lz += Chunk::SIZE;

    // 3. Проверяем, загружен ли чанк
    auto it = m_Chunks.find({cx, cz});
    if (it != m_Chunks.end()) {
        // Ищем самый верхний не-воздушный блок
        for (int y = Chunk::SIZE - 1; y >= 0; y--) {
            if (it->second->GetBlock(lx, y, lz) != BlockType::Air) {
                return y + 1; // Возвращаем координату "над" блоком
            }
        }
    }
    return 0; // Если чанка нет или он пустой
}


// void World::GenerateRegion(int viewDistance) {
//     for (int x = -viewDistance; x <= viewDistance; x++) {
//         for (int z = -viewDistance; z <= viewDistance; z++) {
//             auto chunk = std::make_unique<Chunk>();
//             chunk->SetPosition({x, 0, z});
//
//             chunk->SetupTestWorld();
//
//             m_Chunks[{x, z}] = std::move(chunk);
//         }
//     }
// }



void World::GenerateRegion(int viewDistance) {
    for (int x = -viewDistance; x <= viewDistance; x++) {
        for (int z = -viewDistance; z <= viewDistance; z++) {
            if (m_Chunks.find({x, z}) != m_Chunks.end()) continue;

            auto chunk = std::make_unique<Chunk>();
            chunk->SetWorldPos({x, 0, z});
            GenerateChunkData(chunk.get(), x, z);
            m_Chunks[{x, z}] = std::move(chunk);
        }
    }
}



void World::GenerateChunkData(Chunk* chunk, int cx, int cz) {
    for (int x = 0; x < Chunk::SIZE; x++) {
        for (int z = 0; z < Chunk::SIZE; z++) {
            float wx = (float)(cx * Chunk::SIZE + x);
            float wz = (float)(cz * Chunk::SIZE + z);

            float noiseVal = m_Noise.GetNoise(wx, wz);
            int height = static_cast<int>((noiseVal + 1.0f) * 0.5f * (Chunk::SIZE - 2)) + 2;

            for (int y = 0; y < Chunk::SIZE; y++) {
                if (y == 0) {
                    chunk->SetBlock(x, y, z, BlockType::Bedrock); // Самый низ
                } else if (y < height - 2) {
                    chunk->SetBlock(x, y, z, BlockType::Stone);   // Глубина
                } else if (y < height - 1) {
                    chunk->SetBlock(x, y, z, BlockType::Dirt);    // Подслой
                } else if (y == height - 1) {
                    // Трава или Снег в зависимости от высоты
                    if (height > 12)
                        chunk->SetBlock(x, y, z, BlockType::Snow);
                    else
                        chunk->SetBlock(x, y, z, BlockType::Grass);
                } else {
                    chunk->SetBlock(x, y, z, BlockType::Air);
                }
            }
        }
    }
    chunk->BuildMesh();
}

// void World::Render(Shader* shader) {
//     for (auto& [coords, chunk] : m_Chunks) {
//         // Вычисляем матрицу модели для чанка
//         glm::ivec3 pos = chunk->GetPosition();
//         glm::mat4 model = glm::translate(glm::mat4(1.0f),
//                                          glm::vec3(pos.x * Chunk::SIZE, pos.y * Chunk::SIZE, pos.z * Chunk::SIZE));
//
//         shader->SetMat4("u_Model", model);
//         chunk->Render(shader);
//     }
// }


void World::Render(Shader* shader) {
    for (auto& [coords, chunk] : m_Chunks) {
        // Вызываем Render чанка, он сам разберется с u_Model внутри
        chunk->Render(shader);
    }
}
