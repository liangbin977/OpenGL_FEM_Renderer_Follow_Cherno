#shader vertex
#version 430 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord;

uniform mat4 u_MVP;

void main()
{
   gl_Position = u_MVP * position;
   v_TexCoord = texCoord;
}

#shader fragment
#version 430 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;

void main()
{
   vec4 texColor = texture(u_Texture, v_TexCoord);
   color = texColor; 
   
   // 调试专用：如果还是白屏，取消下面这行的注释
   // color = vec4(v_TexCoord.x, v_TexCoord.y, 0.0, 1.0); 
}







// 这段代码是一个组合了 **顶点着色器 (Vertex Shader)** 和 **片段着色器 (Fragment Shader)** 的 GLSL 源代码文件。

// 需要特别注意的是：**这段代码目前有一个明显的逻辑错误（变量传递缺失），直接运行会报错**。我在解释完每一行后，会在最后给出修复方案。

// 下面是逐行解释：

// ### 1. 顶点着色器 (Vertex Shader) 部分
// 这部分负责处理每一个顶点（顶点的坐标变换等）。

// *   `#shader vertex`
//     *   这不是 GLSL 的标准语法。这是你自己（或者教程作者）在 C++ 代码里写的一个“记号”。C++读取文件时，看到这一行就知道：下面开始是顶点着色器的代码了。
// *   `#version 430 core`
//     *   声明 GLSL 版本为 3.30，且使用 Core Profile（核心模式）。这意味着不能使用那些废弃的老旧函数。
// *   `layout(location = 0) in vec4 position;`
//     *   `in`: 表示这是一个**输入变量**，数据从 CPU（你的 C++ 代码）传进来。
//     *   `vec4`: 变量类型是 4 分量向量 (x, y, z, w)。
//     *   `layout(location = 0)`: 指定了这个属性在“槽位 0”。这必须对应你 C++ 代码里的 `glEnableVertexAttribArray(0)` 和 `glVertexAttribPointer(0, ...)`。
// *   `layout(location = 1) in vec2 TexCoord;`
//     *   定义了**第 2 个输入属性**，位于“槽位 1”。
//     *   `vec2`: 2 分量向量 (u, v)，用来存纹理坐标。
//     *   **注意**：在当前代码中，这个变量被读进来了，但**没有被传给片段着色器**（这是一个 Bug）。
// *   `void main()`
//     *   着色器的主函数入口。
// *   `gl_Position = position;`
//     *   `gl_Position` 是 OpenGL 的内置变量。你必须给它赋值，告诉 GPU这个顶点在屏幕上的最终位置在哪里。这里直接把输入的 `position` 原封不动地赋值给它。

// ---

// ### 2. 片段着色器 (Fragment Shader) 部分
// 这部分负责处理每一个像素的颜色。

// *   `#shader fragment`
//     *   自定义记号，告诉 C++ 解析器：下面是片段着色器的代码了。
// *   `#version 330 core`
//     *   同上，声明版本。
// *   `layout(location = 0) out vec4 color;`
//     *   `out`: 表示这是一个**输出变量**，最终计算出的颜色会通过它输出到屏幕（帧缓冲）。
//     *   `vec4`: 输出的是 RGBA 颜色。
// *   `uniform vec4 u_Color;`
//     *   `uniform`: 表示这是一个**全局变量**。它不是针对每个顶點变化的，而是由 CPU 一次性设置好（`glUniform4f`），在所有像素计算中都保持不变。通常用来做颜色混合或滤镜。
// *   `uniform sampler2D u_Texture;`
//     *   `sampler2D`: 这是一个专门用来操作 2D 纹理的句柄。
//     *   它的值（0, 1, 2...）对应我们在 C++ 里 `glActiveTexture` 选择的那个插槽。
// *   `void main()`
//     *   主函数入口。
// *   `vec4 texColor = texture(u_Texture, v_TexCoord);`
//     *   `texture(...)`: OpenGL 内置函数，用于从纹理中采样颜色。
//     *   参数1 `u_Texture`: 告诉它去哪张图片里找。
//     *   参数2 `TexCoord`: 告诉它去图片的哪个坐标(u, v)取色。
//     *   **错误警告**：这里的 `TexCoord` 在片段着色器里**并没有定义**！它在上面的顶点着色器里定义了，但没有传过来。编译器会在这里报错：`'TexCoord' : undeclared identifier`。
// *   `color = u_Color * texColor;`
//     *   将采样到的纹理颜色（`texColor`）与我们从 CPU 传入的颜色（`u_Color`）相乘。这会产生一种“染色”或“滤镜”的效果。如果 `u_Color` 是白色 (1,1,1,1)，则显示纹理原色。

// ---