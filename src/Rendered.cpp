#include <glad/glad.h>
#include "Renderer.h"
#include <iostream>
const char* GLErrorToString(unsigned int err){
	switch(err){
		case GL_NO_ERROR: return "GL_NO_ERROR";
		case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
		case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
		case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
		case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
		case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
		default: return "Unknown GLError";
	}
}
void GLClearError(){
	while(glGetError() != GL_NO_ERROR);
}
bool GLLogCall(const char* func, const char* file, int line){
	while(unsigned int error = glGetError()){
		std::cout << "[OpenGL Error] (" << GLErrorToString(error) << "): "
		          << func << " " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}
void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const {
	shader.Bind();
	va.Bind();
	ib.Bind();
	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
};
void Renderer::Clear() const {
	GLCall(glClear(GL_COLOR_BUFFER_BIT));
};