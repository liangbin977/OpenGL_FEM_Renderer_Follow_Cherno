#pragma once
#include <string>
class Texture {
    private:
        unsigned int m_RendererID;
        std::string m_FilePath;
        unsigned char* m_LocalBuffer;
        int m_Width, m_Height, m_BPP;
    public:
        Texture(const std::string& path);
        ~Texture();
        void Bind(unsigned int slot = 0) const;
        void Unbind() const;
        unsigned int GetID() const { return m_RendererID; }
        unsigned int GetWidth() const { return m_Width; }
        unsigned int GetHeight() const { return m_Height; }
};