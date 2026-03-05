#include "VertexBuffer.h"
#include "Renderer.h"
VertexBuffer::VertexBuffer(const void* data, unsigned int size){
    // // 创建一个简单的顶点缓冲对象 (VBO)
    GLCall(glGenBuffers(1, &m_RendererID));
    // //绑定缓冲对象，指定它是一个 GL_ARRAY_BUFFER（顶点属性缓冲角色）
	// //"Buffer"（缓冲区）本质上就是显卡（GPU）上的一块内存区域。BUFFER有不同类型。
	// // 绑定（Bind）操作的作用是：告诉 OpenGL，后续对 GL_ARRAY_BUFFER 的所有操作，都是作用在 ID 为 buffer 的这个缓冲对象上的。
	// //GL_ARRAY_BUFFER是VERTEX BUFFER OBJECT的简称，表示这个缓冲区将用于存储顶点属性数据（比如顶点位置、颜色、法线等）。
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    // //把 CPU 数据拷到 GPU
	// //我正在对 GL_ARRAY_BUFFER（顶点缓冲角色）进行“重新分配数据存储”操作，
	// //大小 size，
	// //并用 data 初始化，
	// //usage 给驱动做优化提示。
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}
VertexBuffer::~VertexBuffer(){
    GLCall(glDeleteBuffers(1, &m_RendererID));
}
void VertexBuffer::Bind() const {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}
void VertexBuffer::Unbind() const {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}