#pragma once
#include <glad/glad.h>
#include <string>

class Texture {
public:
    Texture(const std::string& path);
    ~Texture();

    void Bind(uint32_t slot = 0) const;
    void Unbind() const;

    inline uint32_t GetRendererID() const { return m_RendererID; }

    inline int GetWidth() const { return m_Width; }
    inline int GetHeight() const { return m_Height; }

private:
    uint32_t m_RendererID;
    std::string m_FilePath;
    int m_Width, m_Height, m_BPP; // BPP - bytes per pixel
};
