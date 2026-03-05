#include "Test.h"//包含了 Test.h 头文件，定义了 Test 类和 TestMenu 类的接口。
#include "../vendor/imgui/imgui.h" //包含了 ImGui 的核心头文件，提供了 ImGui 的 API 和数据结构。
namespace test {
    TestMenu::TestMenu(Test*& currentTestPointer)
        :m_CurrentTest(currentTestPointer)
    {

    };
    TestMenu::~TestMenu(){};
    void TestMenu::OnImGuiRender(){
        for(auto& test : m_Tests){
            //为每个注册的测试创建一个按钮，按钮的标签是 test.first（测试名称）。当用户点击这个按钮时，就会调用 test.second() 来创建一个新的测试实例，并将其赋值给 m_CurrentTest。
            //绘制按钮的函数是 ImGui::Button，它接受一个字符串作为按钮的标签，并返回一个布尔值，表示按钮是否被点击。
            if(ImGui::Button(test.first.c_str())){
                // 如果按钮被点击，调用存储的函数(test.second)创建一个新测试对象
                // 并赋值给 m_CurrentTest（因为它是引用的指针，所以外部的 currentTest 也会变）
                m_CurrentTest = test.second();
            }
        }
    };
};