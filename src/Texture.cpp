#include <glad/glad.h>
#include "Renderer.h"
#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image/stb_image.h"
//BPP means bits per pixel
Texture::Texture(const std::string& path)
    :m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{

    // OpenGL 的纹理坐标系和图片的坐标系是相反的，所以我们需要在加载图片时进行垂直翻转。
    stbi_set_flip_vertically_on_load(1); 
    //加载图片数据到本地缓冲区，强制要求4个通道（RGBA）
    m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

    //创建OpenGL纹理对象
    GLCall(glGenTextures(1, &m_RendererID));
    //绑定纹理对象到GL_TEXTURE_2D目标
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
    //设置纹理参数
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    //上传纹理数据到GPU
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
    //释放本地缓冲区数据
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
    //释放本地缓冲区数据
    if(m_LocalBuffer)
        stbi_image_free(m_LocalBuffer);
};
Texture::~Texture(){
    GLCall(glDeleteTextures(1, &m_RendererID));
};
//绑定纹理到指定槽位
//槽位一共有32个，从GL_TEXTURE0到GL_TEXTURE31
void Texture::Bind(unsigned int slot) const {
    // 激活指定的纹理插槽并绑定纹理
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    //绑定纹理对象到GL_TEXTURE_2D目标，操作在前面激活的纹理插槽上进行
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}
void Texture::Unbind()const{
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}