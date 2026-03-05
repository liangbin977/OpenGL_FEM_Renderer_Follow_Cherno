#pragma once
class VertexBuffer {
    private:
        unsigned int m_RendererID;
    public:
        //use void* to allow any type of data to be passed in, and size to specify how much data (in bytes) we are passing
        VertexBuffer(const void* data, unsigned int size);
        ~VertexBuffer();
        void Bind() const;
        void Unbind() const;
};