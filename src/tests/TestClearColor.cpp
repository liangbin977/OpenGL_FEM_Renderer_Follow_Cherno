#include "TestClearColor.h"
#include "../Renderer.h"
#include "../vendor/imgui/imgui.h"

namespace test {
    // 构造函数：初始化 m_ClearColor 数组为默认的清屏颜色（蓝色调）
    TestClearColor::TestClearColor()
        :m_ClearColor{0.2f, 0.3f, 0.8f, 1.0f}
    {

    };
    TestClearColor::~TestClearColor(){};
    void TestClearColor::OnUpdate(float deltaTime){}
    //每一帧调用 OnRender 来设置清屏颜色并清除颜色缓冲区
    void TestClearColor::OnRender(){
        // 设置清屏颜色并清除颜色缓冲区
        GLCall(glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]));
        // 设置完清屏颜色后，调用 glClear 来清除颜色缓冲区，使得整个窗口被设置的颜色填充。
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
    }
    // 每一帧调用 OnImGuiRender 来创建一个颜色编辑器控件，允许用户通过 GUI 调整清屏颜色
    void TestClearColor::OnImGuiRender(){
        // ImGui::ColorEdit4 创建一个颜色编辑器控件，允许用户通过 GUI 调整 m_ClearColor 数组中的 RGBA 值。用户调整后，m_ClearColor 的值会被更新，从而在下一帧的 OnRender 中使用新的清屏颜色。
        ImGui::ColorEdit4("Clear Color", m_ClearColor);
    };
};