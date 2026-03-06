#include <glad/glad.h>
#include "Renderer.h"
#include "ImageTexture.h"
ImageTexture::ImageTexture(){
    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
    
    // 【关键点 1】必须设置 Wrap 模式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // 【关键点 2】过滤模式绝对不能带 MIPMAP
    // 如果你有 glGenerateMipmap 还可以用，但这里没有，所以必须是 GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 

    // 【关键点 3】分配显存 (NULL 表示只分配不传数据)
    // 确保 1024x1024 与 DispatchCompute 的参数对应
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 1024, 1024, 0, GL_RGBA, GL_FLOAT, NULL);

    
    glBindTexture(GL_TEXTURE_2D, 0); // 解绑是个好习惯
}
ImageTexture::~ImageTexture(){
    glDeleteTextures(1, &m_RendererID);
}
void ImageTexture::Bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
} 
void ImageTexture::Unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}
void ImageTexture::BindImage(unsigned int slot, GLenum access, GLenum format) const {
    glBindImageTexture(slot, m_RendererID, 0, GL_FALSE, 0, access, format);
}
unsigned int ImageTexture::GetId() const {
    return m_RendererID;
}