#pragma once
#include <functional>
#include <vector>
#include <string>
#include <iostream>

namespace test {
    //基类
    class Test{
        public:
            Test(){}
            virtual ~Test(){}
            virtual void OnUpdate(float deltaTime){}
            virtual void OnRender(){}
            virtual void OnImGuiRender(){}
    };
    //测试菜单类，继承自 Test
    class TestMenu : public Test {
        public:
            TestMenu(Test*& currentTestPointer);
            ~TestMenu();
            void OnImGuiRender()override;
            template<typename T>
            void RegisterTest(const std::string& name){
                std::cout << "Registering test" << name << std::endl;
                //把测试名称和一个创建测试实例的 lambda 函数存储在 m_Tests 向量中。这个 lambda 函数会在用户点击对应按钮时被调用，创建一个新的测试实例。
                m_Tests.push_back(std::make_pair(name, []() { return new T(); }));
            }
        private:
            Test*& m_CurrentTest;
            std::vector<std::pair<std::string, std::function<Test*()>>> m_Tests;                 
    };
};