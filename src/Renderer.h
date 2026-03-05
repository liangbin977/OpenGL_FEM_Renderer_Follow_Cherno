#pragma once
#include <glad/glad.h>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#define GLCall(x) do { GLClearError(); x; GLLogCall(#x, __FILE__, __LINE__); } while(0)
const char* GLErrorToString(unsigned int err);
void GLClearError();
bool GLLogCall(const char* func, const char* file, int line);
class Renderer {
    public:
        void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader)const;
        void Clear() const;
};