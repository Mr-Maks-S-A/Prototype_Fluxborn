/**
 * @file Texture.hpp
 * @brief Обертка над текстурным объектом OpenGL.
 */

#pragma once
#include <glad/glad.h>
#include <string>

/**
 * @class Texture
 * @brief Загружает изображение с диска и создает текстурный объект в видеопамяти.
 * * Оптимизирована для работы с текстурными атласами (Pixel Art).
 * * Использует библиотеку stb_image для декодирования форматов (PNG, JPG и др.).
 */
class Texture {
public:
    /**
     * @brief Создает текстуру из файла.
     * @param path Путь к файлу изображения (относительно корня проекта).
     * @note По умолчанию форсирует 4 канала (RGBA) для совместимости.
     */
    Texture(const std::string& path);

    /** @brief Удаляет текстуру из видеопамяти GPU. */
    ~Texture();

    /**
     * @brief Привязывает текстуру к указанному слоту.
     * @param slot Индекс текстурного юнита (по умолчанию 0).
     */
    void Bind(uint32_t slot = 0) const;

    /** @brief Отвязывает текущую текстуру. */
    void Unbind() const;

    /** @name Геттеры */
    ///@{
    inline uint32_t GetRendererID() const { return m_RendererID; }
    inline int GetWidth() const { return m_Width; }
    inline int GetHeight() const { return m_Height; }
    ///@}

private:
    uint32_t m_RendererID; ///< Внутренний ID объекта OpenGL.
    std::string m_FilePath; ///< Путь к исходному файлу (для дебага).
    int m_Width, m_Height, m_BPP;
};
