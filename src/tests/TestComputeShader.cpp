#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "TestComputeShader.h"
#include "../Renderer.h"
#include "../vendor/imgui/imgui.h"
#include <iostream>
#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtc/matrix_transform.hpp"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"

namespace test {
    ImageTexture2D::ImageTexture2D()
        :m_TranslationA(0, 0, 0), m_TranslationB(0.5f, 0.5f, 0), m_Proj(glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f)), m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0.1, 0, 0)))
    {
        float positionsp[] = {
            -0.5f, -0.5f, 0, 0,//0
            0.5f, -0.5f, 1, 0,//1
            0.5f,  0.5f, 1, 1,//2
            -0.5f,  0.5f, 0, 1//3
        };  
        unsigned int indices[6] = {
            0,1,2,
            2,3,0
        };

        m_VAO = std::make_unique<VertexArray>();
        m_VBO = std::make_unique<VertexBuffer>(positionsp, sizeof(positionsp));
        //只有 EBO (GL_ELEMENT_ARRAY_BUFFER) 会被 绑定状态的VAO 自动保存绑定状态。
        //VBO (GL_ARRAY_BUFFER) 不会被 VAO 保存绑定状态。VBO 是通过 glVertexAttribPointer 将
        //具体的缓冲 ID 链接到具体的属性槽（Attribute Slot）上的，而不是存绑定状态。
        //总结：只要你的 IBO 是在 VAO 处于 Bind 状态期间创建或绑定的，va.Bind() 就会自动把它带回来。
        // 【关键规则】：VAO 只有在自己处于 Bind 状态下，
        // 才会把“当前绑定的 GL_ELEMENT_ARRAY_BUFFER”记录下来。
        // 因为 va.AddBuffer 里调用了 va.Bind() 且没有 Unbind，
        // 所以此时 VAO 是绑定状态。
        // IndexBuffer 构造函数里执行 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ...)，这会被 VAO 记下来。
        m_IBO = std::make_unique<IndexBuffer>(indices, 6);

        m_Layout = std::make_unique<VertexBufferLayout>();
        m_Layout->Push<float>(2);//0: position
        m_Layout->Push<float>(2);//1: texture coord
        m_VAO->AddBuffer(*m_VBO, *m_Layout);

        m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
        //需要与vertex和fragment shader分开定义
        m_ComputeShader = std::make_unique<Shader>("res/shaders/Compute.shader");

        m_ImageTexture = std::make_unique<ImageTexture>();
    };
    ImageTexture2D::~ImageTexture2D(){};
    void ImageTexture2D::OnUpdate(float deltaTime){}
    void ImageTexture2D::OnRender(){
 // 1. 清屏
    GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT));

    // ====================================================
    // 阶段一：计算 (Compute Shader)
    // ====================================================
    {
        // 1. 必须先绑定 Shader，才能设置它的 Uniform
        m_ComputeShader->Bind();
        
        // 2. 绑定图像单元 (用于写入)
        m_ImageTexture->BindImage(0);

        // 3. 传参 & 调度
        m_ComputeShader->SetUniform1f("u_Time", (float)glfwGetTime()); 
        glDispatchCompute(512/8, 512/8, 1); // 确保覆盖全图

        // 4.哪怕是同一帧内读写，也要加屏障
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    }    
    {
        // 绑定纹理
        glBindTexture(GL_TEXTURE_2D, m_ImageTexture->GetId());
        
        // 准备一个 buffer 接数据
        float* pixels = new float[512 * 512 * 4];
        
        // 从显存把纹理读回来 (这一步非常暴力，会卡顿，但为了调试值得)
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, pixels);
        
        // 检查中心点像素 (256, 256)
        int index = (256 * 512 + 256) * 4;
        float r = pixels[index + 0];
        float g = pixels[index + 1];
        float b = pixels[index + 2];
        float a = pixels[index + 3];
        
        // 打印颜色值 (只打印一次，防止刷屏)
        static bool s_Logged = false;
        if (!s_Logged) {
            std::cout << ">>> GPU Texture Center Pixel: (" 
                      << r << ", " << g << ", " << b << ", " << a << ")" << std::endl;
            s_Logged = true;
        }

        delete[] pixels;
    }

    // ====================================================
    // 阶段二：显示 (Graphic Shader)
    // ====================================================
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        Renderer renderer;
        // 【重点修复】必须先绑定图形 Shader，后续的 SetUniform 才有效！
        m_Shader->Bind();

        // 1. 绑定纹理
        m_ImageTexture->Bind(0);            // 绑定到 0 号槽
        m_Shader->SetUniform1i("u_Texture", 0); // 告诉 Shader 读 0 号槽

        // 2. 绘制物体 A (响应 ImGui m_TranslationA)
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
            glm::mat4 mvp = m_Proj * m_View * model; 
            
            // 因为上面已经调用了 m_Shader->Bind()，这里设置矩阵才会生效
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            
            // Draw 内部通常会再次 Bind Shader，这也是安全的
            renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
        }

        // 3. 绘制物体 B (响应 ImGui m_TranslationB)
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);
            glm::mat4 mvp = m_Proj * m_View * model; 
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
        }
    }
    }
    void ImageTexture2D::OnImGuiRender(){
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);
		// ImGui::SliderFloat() 函数创建了一个滑动条控件，允许用户调整一个浮点数值。参数含义如下：
        ImGui::SliderFloat("Move X", &m_TranslationA.x, -2.0f, 2.0f);
		ImGui::SliderFloat("Move Y", &m_TranslationA.y, -1.5f, 1.5f);
		ImGui::SliderFloat("Move Z", &m_TranslationA.z, -1.0f, 1.0f);
		ImGui::SliderFloat("MoveB X", &m_TranslationB.x, -2.0f, 2.0f);
		ImGui::SliderFloat("MoveB Y", &m_TranslationB.y, -1.5f, 1.5f);
		ImGui::SliderFloat("MoveB Z", &m_TranslationB.z, -1.0f, 1.0f); 
		// Edit 1 float using a slider from 0.0f to 1.0f
        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::End();    
    };
};