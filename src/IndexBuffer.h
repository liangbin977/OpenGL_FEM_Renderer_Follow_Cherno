#pragma once
class IndexBuffer {
    private:
        unsigned int m_RendererID;
        unsigned int m_Count;
    public:
        //use void* to allow any type of data to be passed in, and size to specify how much data (in bytes) we are passing
        IndexBuffer(const unsigned int* data, unsigned int count);
        ~IndexBuffer();
        void Bind() const;
        void Unbind() const;
        inline unsigned int GetCount() const { return m_Count; }
};