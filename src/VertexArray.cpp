#include <glad/glad.h>
#include "VertexArray.h"
#include "Renderer.h"
struct VertexBufferElement; 
VertexArray::VertexArray(){
    glGenVertexArrays(1, &m_RendererID);
};
VertexArray::~VertexArray(){};
void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& Layout) const {
    Bind();
    const auto& elements = Layout.GetElements();
    unsigned int offset = 0;
    for(unsigned int i = 0; i < elements.size(); i++){
        const auto& element = elements[i];
        /*
        为什么不能删除vb.bind()这行代码？
        这是一个非常敏锐的问题！说明你正在认真思考代码的执行流程。
        答案是：**在你的 main.cpp 这几行简单的代码里，你是对的，它是“多余”的。但是，作为封装好的 `VertexArray` 类，`vb.Bind()` 必须写，不能删。**
        为什么？因为我们不能假设 main.cpp 里永远只创建一个 VBO，或者在创建 VBO 和调用 `AddBuffer` 之间没有发生其他操作。
        ### 场景推演：为什么必须写？
        试想一下，如果你以后写了稍微复杂一点的代码，有两个 VBO：

        ```cpp
        // 1. 创建第一个 VBO
        VertexBuffer vb1(data1, size1); // 此时 OpenGL 绑定的是 vb1 (ID=1)

        // 2. 创建第二个 VBO
        VertexBuffer vb2(data2, size2); // 此时 OpenGL 绑定由 vb1 变成了 vb2 (ID=2)！！！

        // 3. 配置 vb1 的布局
        VertexArray va;
        VertexBufferLayout layout;
        // ... 配置 layout ...

        // 关键时刻：调用 AddBuffer 传入 vb1
        va.AddBuffer(vb1, layout); 
        ```

        **如果你的 `AddBuffer` 里没有 `vb.Bind()`：**
        1.  `AddBuffer` 内部先绑定了 VAO。
        2.  然后调用 `glVertexAttribPointer`。
        3.  **灾难发生**：`glVertexAttribPointer` 会去读取**当前绑定在 GL_ARRAY_BUFFER 上的 VBO**。
        4.  此时绑定的是谁？是 **vb2**（因为它是最后创建的）！
        5.  **结果**：你的 VAO 错误地记录了 vb2 的数据来源，而不是你传入的 vb1。渲染 vb1 时会导致数据错乱或崩溃。

        ### 核心原理：就近原则
        `glVertexAttribPointer` 这行指令就像一个“快照相机”。
        *   它不接受 VBO ID 作为参数。
        *   它只拍**当前** `GL_ARRAY_BUFFER` 绑定的是谁。

        为了确保拍到的是正确的 VBO，`AddBuffer` 函数必须对自己负责：**“不管外面发生了什么，既然你让我把 `vb` 加入 VAO，我就得在配置前强制把 `vb` 绑上去，确保万无一失。”**

        ### 结论
        虽然在你当前的 `main` 函数里，因为逻辑很简单（创建vb -> 马上AddBuffer），看起来是重复绑定了。

        但在封装类的设计原则里，**函数不应依赖“调用者之前的状态”**。

        所以，请保留 `VertexArray.cpp` 里的这段逻辑：

        ```cpp
        void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& Layout) const {
            Bind();     // 绑定 VAO
            vb.Bind();  // 绑定 VBO (必须写！为了防止在外面绑了别的 VBO)
            
            // ... loop glVertexAttribPointer ...
        }
        */
        vb.Bind();
        // //启用顶点属性数组，参数 0 是我们要启用的顶点属性的索引。
        GLCall(glEnableVertexAttribArray(i));
        // //glVertexAttribPointer 用于定义如何解析顶点数据。
	    // //第一个参数是我们要设置的顶点属性（位置，坐标系等）的索引（在顶点着色器中用 layout(location = 0) 指定的）。
	    // //第二个参数是每个顶点属性的组件数量（这里是 2，因为每个位置有 x 和 y 两个分量）。
	    // //第三个参数是数据类型（GL_FLOAT 表示浮点数）。
	    // //第四个参数指定是否要归一化数据（GL_FALSE 表示不归一化）。
	    // //第五个参数是步长（stride），表示连续顶点属性之间的字节数（这里是 sizeof(float)*2，因为每个顶点有两个 float）。
	    // //第六个参数是数据在缓冲区中的偏移量（offset），这里是 0，表示从缓冲区的开头开始读取数据）。
        GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, Layout.GetStride(),  (const void*)(uintptr_t)offset));
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }
}
void VertexArray::Bind() const {
    GLCall(glBindVertexArray(m_RendererID));
}
void VertexArray::Unbind() const {
    GLCall(glBindVertexArray(0));
}