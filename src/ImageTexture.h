#pragma once
class ImageTexture {
    private:
        unsigned int m_RendererID;
    public:
        ImageTexture();
        ~ImageTexture();
        void Bind(unsigned int slot = 0) const;
        void Unbind() const;
        void BindImage(unsigned int slot = 0) const;
        unsigned int GetId() const;
};