#include "TestTexture2D.h"
#include "../Renderer.h"
#include "../vendor/imgui/imgui.h"

#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtc/matrix_transform.hpp"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"
#include <iostream>
namespace test {
    TestTexture2D::TestTexture2D()
        :m_TranslationA(0, 0, 0),m_Proj(glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f)), m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0.1, 0, 0)))
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
        m_Shader->Bind();
        std::cout << ">>> Shader Program ID: " << m_Shader->GetRendererID() << std::endl;

        m_Texture = std::make_unique<Texture>("res/Textures/C.png");
        m_Shader->SetUniform1i("u_Texture", 0); // 纹理槽 0 对应 GL_TEXTURE0
        //这行代码创建了一个正交投影矩阵，定义了一个从 -2.0 到 2.0（水平）和 -1.5 到 1.5（垂直）的视口范围，近裁剪面距离为 -1.0，远裁剪面距离为 1.0。
        //等效于把视野拉远了一些，能看到更多内容。（默认是-1到1）
        // m_Proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
        // 这行代码创建了一个视图矩阵，表示从世界坐标系向右平移 1 个单位的变换。它相当于把摄像机向左移动了 1 个单位。
        //观测变换的本质是：建立一个新的坐标系，该坐标系以 eye 为坐标原点，以 s, u, -f 为 x 轴、 y 轴、z 轴，将原向量 v 映射到新坐标系下的向量 u
        //x坐标表示右方向，y坐标表示上方向，z坐标表示远近方向（负 z 轴朝向观察者）。因此，translate 的参数是一个 vec3，表示在 x、y、z 方向上的平移量。
        // m_View = glm::translate(glm::mat4(1.0f), glm::vec3(0.1, 0, 0));

    };
    TestTexture2D::~TestTexture2D(){};
    void TestTexture2D::OnUpdate(float deltaTime){}
    void TestTexture2D::OnRender(){
        GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
        //开启透明度
	    glEnable(GL_BLEND);
	    //设置混合函数，指定如何计算源颜色和目标颜色的混合结果
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        Renderer renderer;
        m_Texture->Bind(0);
        {//view 和 model 的区别：view 是从世界坐标系到观察坐标系的变换，model 是从模型坐标系到世界坐标系的变换。view 负责把整个场景移动、旋转、缩放到摄像机的位置和方向，而 model 负责把单个物体移动、旋转、缩放到它在世界中的位置和姿态。
		glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
		glm::mat4 mvp = m_Proj * m_View * model; // 注意矩阵乘法的顺序：先 model，再 view，最后 proj。
		m_Shader->Bind();
        //设置投影矩阵
		m_Shader->SetUniformMat4f("u_MVP", mvp);
	
		// 绘制
		renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
		};
    }
    void TestTexture2D::OnImGuiRender(){
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);
		// ImGui::SliderFloat() 函数创建了一个滑动条控件，允许用户调整一个浮点数值。参数含义如下：
        ImGui::SliderFloat("Move X", &m_TranslationA.x, -2.0f, 2.0f);
		ImGui::SliderFloat("Move Y", &m_TranslationA.y, -1.5f, 1.5f);
		ImGui::SliderFloat("Move Z", &m_TranslationA.z, -1.0f, 1.0f);
		// Edit 1 float using a slider from 0.0f to 1.0f
        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::End();    
    };
};