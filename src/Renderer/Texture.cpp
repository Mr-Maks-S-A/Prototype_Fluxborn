#include "Renderer/Texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

Texture::Texture(const std::string& path)
: m_RendererID(0), m_FilePath(path), m_Width(0), m_Height(0), m_BPP(0)
{
    // В OpenGL текстуры обычно перевернуты по Y относительно stb_image
    stbi_set_flip_vertically_on_load(1);

    unsigned char* localBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

    if (!localBuffer) {
        std::cerr << "[Texture Error] Failed to load: " << path << std::endl;
        return;
    }

    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);

    // Настройки для Pixel Art (Pixel Perfect)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (localBuffer)
        stbi_image_free(localBuffer);
}

Texture::~Texture() {
    glDeleteTextures(1, &m_RendererID);
}

void Texture::Bind(uint32_t slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture::Unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}
