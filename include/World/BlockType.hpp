#pragma once
#include <cstdint>

/**
 * @enum BlockType
 * @brief Типы блоков. ID соответствует индексу в текстурном атласе.
 * 0 - это всегда воздух (не рендерится).
 */
enum class BlockType : uint8_t {
    Air         = 0,
    Grass       = 1,
    Dirt        = 2,
    Stone       = 3,
    Sand        = 4,
    Snow        = 5,
    Water       = 6, // Или лед
    OakWood     = 7,
    Leaves      = 8,
    Brick       = 9,
    Cobblestone = 10,
    Gold        = 11,
    Iron        = 12,
    Teal        = 13, // Магический блок
    Bedrock     = 14,
    Error       = 15,// Розовая текстура для дебага
};

struct UVRect {
    float u, v; // Левый верхний угол
};

inline UVRect GetUVForID(int texID) {
    float atlasSize = 256.0f;
    float blockSize = 16.0f;
    float normalizedStep = blockSize / atlasSize; // 16/256 = 0.0625

    // Так как все 16 блоков в первом ряду (y = 0)
    float u = (texID % 16) * normalizedStep;
    float v = (texID / 16) * normalizedStep;

    return { u, v };
}
