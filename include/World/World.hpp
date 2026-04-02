/**
 * @file World.hpp
 * @brief Менеджер игрового пространства Fluxborn.
 */

#pragma once
#include <map>
#include <set>
#include <tuple>
#include <memory>
#include "World/Chunk.hpp"
#include "Core/ThreadPool.hpp"
#include "FastNoiseLite.h"

/**
 * @class World
 * @brief Управляет коллекцией чанков, их генерацией и жизненным циклом.
 * * Класс служит связующим звеном между процедурной генерацией (шум Перлина)
 * и физическим представлением мира в памяти.
 */
class World {
public:
    /**
     * @brief Конструктор мира.
     * @param seed Зерно генератора для создания уникального ландшафта.
     */
    World(int seed);

    ~World() = default;

    /**
     * @brief Определяет высоту поверхности в мировых координатах.
     * * Используется системой коллизий для предотвращения проваливания игрока сквозь землю.
     * @param worldX Координата X в мировом пространстве.
     * @param worldZ Координата Z в мировом пространстве.
     * @return int Высота (Y) самого верхнего непрозрачного блока + 1.
     */
    int GetHeightAt(int worldX, int worldZ);

    /**
     * @brief Генерирует регион чанков вокруг центра мира.
     * @param viewDistance Радиус генерации (в чанках).
     * @note На данный момент работает синхронно, что может вызвать фризы при больших значениях.
     */
    void GenerateRegion(int viewDistance);

    /**
     * @brief Отрисовывает все загруженные чанки.
     * @param shader Указатель на активный шейдер для передачи матриц трансформации.
     */
    void Render(Shader* shader);

    /**
     * @brief Опрашивает очередь готовых данных и создает для них меши.
     * Вызывается каждый кадр в основном цикле.
     */
    void UpdateAsyncGeneration();

private:
    /**
     * @brief Процедурное заполнение данных блоков внутри конкретного чанка.
     * * Алгоритм:
     * 1. Рассчитывает высоту ландшафта через FastNoiseLite.
     * 2. Заполняет слои: Bedrock -> Stone -> Dirt -> Grass/Snow.
     * 3. Вызывает построение меша (BuildMesh) для GPU.
     * * @param chunk Указатель на объект чанка.
     * @param cx Координата чанка по X.
     * @param cz Координата чанка по Z.
     */
    void GenerateChunkData(Chunk* chunk, int cx, int cz);

private:
    FastNoiseLite m_Noise; ///< Генератор шума для ландшафта.
    int m_Seed;            ///< Зерно генерации.

    /** * @brief Контейнер активных чанков.
     * Ключ: пара координат {x, z}.
     * @todo Заменить на flat_map или хэш-таблицу для улучшения кэш-локальности (DoD).
     */
    std::map<std::pair<int, int>, std::unique_ptr<Chunk>> m_Chunks;

    // Поток для задач
    std::unique_ptr<ThreadPool> m_Pool;



    // Список координат чанков, которые сейчас находятся в процессе генерации
    // Чтобы не заказать один и тот же чанк дважды
    std::set<std::pair<int, int>> m_LoadingChunks;
    std::mutex m_LoadingMutex;


    struct PendingChunk {
        std::unique_ptr<Chunk> chunk;
        Chunk::MeshData meshData;
    };

    // Очередь чанков, которые уже заполнены блоками, но еще не имеют меша
    std::mutex m_FinishedMutex;
    std::queue<PendingChunk> m_FinishedChunks; // Очередь из пар [Объект + Данные]

    std::mutex m_ChunksMutex;
};
