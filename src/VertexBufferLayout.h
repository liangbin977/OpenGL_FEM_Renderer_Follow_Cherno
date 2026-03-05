// #include <glad/glad.h>
// #include <vector>
// #include <type_traits>
// struct VertexBufferElement {
//     unsigned int count;
//     unsigned int type;
//     unsigned char normalized;
//     static unsigned int GetSizeOfType(unsigned int type){
//         switch(type){
//             case GL_FLOAT:return 4;
//             case GL_UNSIGNED_INT:return 4;
//             case GL_UNSIGNED_BYTE:return 1;
//         }
//         return 0;
//     }
// };
// class VertexBufferLayout {
//     private:
//         std::vector<VertexBufferElement> m_Elements;
//         unsigned int m_Stride;
//     public:
//         VertexBufferLayout()
//             :m_Stride(0) {};
//         ~VertexBufferLayout();
//         template<typename T>
//         void Push(int count){
//             static_assert(false);
//         };
//         template<>
//         void Push<float>(unsigned int count){
//             m_Elements.push_back({count, GL_FLOAT, GL_FALSE});
//             m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
//         };
//         template<>
//         void Push<unsigned int>(unsigned int count){
//             m_Elements.push_back({count, GL_UNSIGNED_INT, GL_FALSE});
//             m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
//         };
//         template<>
//         void Push<unsigned char>(unsigned int count){
//             m_Elements.push_back({count, GL_UNSIGNED_BYTE, GL_FALSE});
//             m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
//         };
//         inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements;}
//         inline const unsigned int GetStride() const { return m_Stride;}
        
// };

#pragma once
#include <glad/glad.h>
#include <vector>
#include <type_traits>

struct VertexBufferElement {
    unsigned int count;
    unsigned int type;
    unsigned char normalized;

    static unsigned int GetSizeOfType(unsigned int type) {
        switch (type) {
            case GL_FLOAT:         return 4;
            case GL_UNSIGNED_INT:  return 4;
            case GL_UNSIGNED_BYTE: return 1;
            default:               return 0;
        }
    }
};

// Traits：把 C++ 类型映射到 OpenGL 的类型枚举与 normalized
template<typename T>
struct VBETraits; // 不定义：未支持的类型会在编译期报错

template<>
struct VBETraits<float> {
    static constexpr unsigned int Type = GL_FLOAT;
    static constexpr unsigned char Normalized = GL_FALSE;
};

template<>
struct VBETraits<unsigned int> {
    static constexpr unsigned int Type = GL_UNSIGNED_INT;
    static constexpr unsigned char Normalized = GL_FALSE;
};

template<>
struct VBETraits<unsigned char> {
    static constexpr unsigned int Type = GL_UNSIGNED_BYTE;
    static constexpr unsigned char Normalized = GL_TRUE; // 常见约定：byte 颜色通常要归一化到 0..1
};

class VertexBufferLayout {
private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;

public:
    VertexBufferLayout() : m_Stride(0) {}

    template<typename T>
    void Push(unsigned int count) {
        // 如果 VBETraits<T> 没有特化，这里会编译失败（提示类型不支持）
        m_Elements.push_back({ count, VBETraits<T>::Type, VBETraits<T>::Normalized });
        //m_Stride表示的是每个顶点的总字节数，每次添加一个属性，就要增加这个属性占用的字节数
        m_Stride += count * VertexBufferElement::GetSizeOfType(VBETraits<T>::Type);
    }

    inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }
    inline unsigned int GetStride() const { return m_Stride; }
};