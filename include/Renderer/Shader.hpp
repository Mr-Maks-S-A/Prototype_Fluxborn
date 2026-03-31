/**
 * @file Shader.hpp
 * @brief Класс для управления шейдерными программами OpenGL.
 */

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <unordered_map>

/**
 * @class Shader
 * @brief Инкапсулирует компиляцию, линковку и передачу данных (uniforms) в GPU.
 * * Автоматизирует чтение .vert и .frag файлов.
 * * Предоставляет удобный интерфейс для установки Uniform-переменных.
 */
class Shader {
public:
    /**
     * @brief Загружает, компилирует и линкует шейдерную программу.
     * @param vertexPath Путь к вершинному шейдеру.
     * @param fragmentPath Путь к фрагментному шейдеру.
     */
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    /** @brief Удаляет программу из памяти GPU. */
    ~Shader();

    /** @brief Активирует шейдерную программу (glUseProgram). */
    void Bind() const;

    /** @brief Деактивирует текущую программу. */
    void Unbind() const;

    /** @name Утилиты Uniform-переменных
     * Методы для передачи данных (матрицы трансформации, цвета, текстурные слоты).
     */
    ///@{
    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetVec3(const std::string& name, const glm::vec3& value);
    void SetVec4(const std::string& name, const glm::vec4& value);
    void SetMat4(const std::string& name, const glm::mat4& mat);
    ///@}

private:
    /**
     * @brief Внутренняя проверка статуса компиляции/линковки.
     * @param shader ID объекта OpenGL.
     * @param type Строка с типом (VERTEX/FRAGMENT/PROGRAM) для лога.
     */
    void CheckCompileErrors(unsigned int shader, std::string type);

    uint32_t m_RendererID; ///< Внутренний ID программы в OpenGL.

    /** * @todo Реализовать использование m_UniformLocationCache.
     * Вызов glGetUniformLocation каждый кадр — дорогая операция.
     * Кэширование локаций в unordered_map ускорит рендеринг чанков.
     */
    std::unordered_map<std::string, int> m_UniformLocationCache;
};
