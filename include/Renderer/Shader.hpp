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
 * @brief Инкапсулирует компиляцию, линковку и передачу данных (uniforms) в шейдер.
 */
class Shader {
public:
    /**
     * @brief Загружает и компилирует шейдер из файлов.
     * @param vertexPath Путь к вершиному шейдеру (.vert)
     * @param fragmentPath Путь к фрагментному шейдеру (.frag)
     */
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    /** @name Утилиты Uniform-переменных
     * Методы для передачи данных из CPU в GPU (матрицы, векторы, цвета).
     */
    ///@{
    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetVec3(const std::string& name, const glm::vec3& value);
    void SetMat4(const std::string& name, const glm::mat4& mat);
    void SetVec4(const std::string& name, const glm::vec4& value);
    ///@}

private:
    /**
     * @brief Проверка ошибок компиляции и линковки.
     * @param shader ID шейдера или программы.
     * @param type Тип (PROGRAM, VERTEX, FRAGMENT).
     */
    void CheckCompileErrors(unsigned int shader, std::string type);

    uint32_t m_RendererID; ///< ID шейдерной программы в OpenGL.
    std::unordered_map<std::string, int> m_UniformLocationCache; ///< Кэш локаций для оптимизации.
};
