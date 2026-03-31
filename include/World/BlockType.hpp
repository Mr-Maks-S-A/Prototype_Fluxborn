/**
 * @file BlockType.hpp
 * @brief Определение типов вокселей и логики работы с атласом.
 */

#pragma once
#include <cstdint>

/**
 * @enum BlockType
 * @brief Идентификаторы типов блоков.
 * * Значение соответствует индексу в текстурном атласе (16x16 ячеек).
 * * Использование uint8_t критически важно для экономии памяти в массивах чанков.
 */
enum class BlockType : uint8_t {
    Air         = 0,  ///< Прозрачный блок (не имеет меша).
    Grass       = 1,  ///< Трава (использует разные текстуры для верха/низа/боков).
    Dirt        = 2,  ///< Земля.
    Stone       = 3,  ///< Камень.
    Sand        = 4,  ///< Песок.
    Snow        = 5,  ///< Снег.
    Water       = 6,  ///< Вода (потребует отдельного прохода рендеринга для прозрачности).
    OakWood     = 7,  ///< Дерево (дуб).
    Leaves      = 8,  ///< Листва.
    Brick       = 9,  ///< Кирпич.
    Cobblestone = 10, ///< Булыжник.
    Gold        = 11, ///< Золотая руда/блок.
    Iron        = 12, ///< Железная руда/блок.
    Teal        = 13, ///< Магический/технический блок.
    Bedrock     = 14, ///< Коренная порода (неразрушимый блок).
    Error       = 15  ///< Отладочный блок (ярко-розовый для поиска багов).
};

/**
 * @struct UVRect
 * @brief Хранит нормализованные координаты (0.0f - 1.0f) левого верхнего угла текстуры.
 */
struct UVRect {
    float u, v;
};

/**
 * @brief Вычисляет UV-смещение для текстурного атласа.
 * * Алгоритм предполагает, что атлас имеет размер 256x256 пикселей
 * и состоит из плиток 16x16.
 * @param texID Индекс текстуры (0-255).
 * @return UVRect Координаты для шейдера.
 */
inline UVRect GetUVForID(int texID) {
    // В будущем эти константы можно вынести в настройки RendererAPI
    constexpr float atlasSize = 256.0f;
    constexpr float blockSize = 16.0f;
    constexpr float normalizedStep = blockSize / atlasSize; // 0.0625f

    float u = (texID % 16) * normalizedStep;
    float v = (texID / 16) * normalizedStep;

    return { u, v };
}
