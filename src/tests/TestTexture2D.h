#pragma once
#include "Test.h"
#include "../VertexBuffer.h"
#include "../IndexBuffer.h"
#include "../VertexArray.h"
#include "../Shader.h"
#include "../Texture.h"
#include <memory>
namespace test {
    class TestTexture2D: public Test {
        public:
            TestTexture2D();
            ~TestTexture2D();
            void OnUpdate(float deltaTime) override;
            void OnRender() override;
            void OnImGuiRender() override;
        private:
            glm::vec3 m_TranslationA;
            bool show_demo_window = true;
            bool show_another_window = false;

            glm::mat4 m_Proj, m_View;

            std::unique_ptr<VertexBuffer> m_VBO;
            std::unique_ptr<VertexArray> m_VAO;
            std::unique_ptr<IndexBuffer> m_IBO;
            std::unique_ptr<VertexBufferLayout> m_Layout;
            std::unique_ptr<Shader> m_Shader;
            std::unique_ptr<Texture> m_Texture;
    };
};
