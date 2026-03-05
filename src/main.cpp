//项目配置教程具体看bilibili
/*
读代码的 4 个抓手（立刻见效）
1) 把所有 GL 调用按“角色”分组理解
你现在这份代码基本就 5 类调用：

创建/销毁对象：glCreateShader/glCreateProgram/glGenBuffers/glDeleteShader...
绑定到某个槽位（Target）：glBindBuffer(GL_ARRAY_BUFFER, ...)（关键：绑定会影响后续很多函数）
往对象里塞数据/代码：glBufferData/glShaderSource
配置解释方式/管线状态：glVertexAttribPointer/glEnableVertexAttribArray
使用 + 绘制：glUseProgram/glDrawArrays
阅读时不要被函数名淹没，只问三件事：
“这行在操作哪个对象？它依赖哪个绑定槽位？它改变了什么状态？”


*/
#include <glad/glad.h>  // 必须放在第一行！它包含了 OpenGL 的头文件。如果放在 GLFW 后面，编译器会报错。
#include <GLFW/glfw3.h> // 处理窗口创建、键盘鼠标输入和 OpenGL 上下文管理。
#include <iostream>    // 用于输出错误信息到控制台。
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

#include "tests/TestClearColor.h"
#include "tests/TestTexture2D.h"
#include "tests/TestComputeShader.h"
#include "tests/Test.h"

// struct ShaderProgramSource{
// 	std::string VertexSource;
// 	std::string FragmentSource;
// };


int main(void){
    GLFWwindow* window; // 声明一个指针，用来引用我们要创建的窗口对象。

    // 1. 初始化 GLFW 库
    // 在调用任何其他 GLFW 函数之前，必须先调用它。
    if(!glfwInit())
        return -1; // 如果初始化失败（返回0），程序直接退出，返回错误码 -1。
    // 必须在 glfwInit() 之后，glfwCreateWindow 之前
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // 必须是 4
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // 必须是 3 或更高 (4.3 / 4.5 / 4.6)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // 2. 创建窗口
    // 参数含义：宽度(640), 高度(480), 标题("Hello World"), 显示器(NULL表示窗口模式), 共享上下文(NULL表示不共享)
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    // 检查窗口是否创建成功（例如驱动不支持或内存不足时会返回 NULL）
    if(!window){
        glfwTerminate(); // 创建失败时，清理之前 glfwInit 分配的资源
        return -1;
    }
    // 3. 设置当前 OpenGL 上下文 (Context)
    // 这是一个关键步骤。它告诉 OpenGL：“接下来的所有绘图指令，都是作用在这个 window 上的”。
    // 必须在初始化 GLAD 之前调用它。
    glfwMakeContextCurrent(window);
	glfwSwapInterval(1); //启用垂直同步（V-Sync），限制帧率与显示器刷新率一致，防止画面撕裂。
    // 4. 初始化 GLAD (加载 OpenGL 函数指针)
    // OpenGL 的函数位置在运行时是由显卡驱动决定的，GLAD 的作用就是去查找这些函数的地址。
    // 我们把 GLFW 获取函数地址的方法 (glfwGetProcAddress) 传给 GLAD。
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        return -1; // 如果 GLAD 初始化失败（通常是因为上下文没建立好），退出程序。
    }
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	//use {} to limit the scope of variables, or claim memory from heap.

	Renderer renderer;

	//imgui初始化标准流程
	//必须在创建 OpenGL 上下文（glfwMakeContextCurrent）和加载 OpenGL 函数指针（gladLoadGLLoader）之后调用 ImGui::CreateContext() 来创建 ImGui 上下文。
	//创建上下文后，可以通过 ImGui::GetIO() 获取 ImGuiIO
	ImGui::CreateContext();
	// ImGuiIO 结构体包含了 ImGui 的输入输出配置和状态信息。通过它可以设置一些全局的 ImGui 配置选项，比如启用键盘导航、设置显示尺寸、配置字体等。
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// 初始化 ImGui 的 GLFW 和 OpenGL3 后端，告诉它们我们使用的窗口和 OpenGL 版本。
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 430");
	ImGui::StyleColorsDark();

/*
	//用于变色的变量
	float  r = 0; 
	float increment = 0.05f;
*/
	//测试框架
	test::Test* currentTest = nullptr;
	//创建一个 TestMenu 实例，并将 currentTest 的地址传给它。TestMenu 是一个测试菜单类，允许用户在运行时选择不同的测试用例。
	test::TestMenu* testMenu = new test::TestMenu(currentTest);
	//把菜单的指针传给 currentTest，这样在菜单里选择不同测试时，currentTest 就会被切换到不同的测试实例。
	currentTest = testMenu;
	//注册测试用例，这里注册了一个 TestClearColor 测试用例，用户在菜单里选择它时，就会创建一个 TestClearColor 实例并切换到它。
	testMenu ->RegisterTest<test::TestClearColor>("Clear Color");
	testMenu ->RegisterTest<test::TestTexture2D>("2D Texture");
    testMenu ->RegisterTest<test::ImageTexture2D>("Compute Shader");
    while(!glfwWindowShouldClose(window)){
        // 1. 清屏
		//刷满目标颜色
		GLCall(glClearColor(0, 0, 0, 1.0f));
		renderer.Clear();
        
		//每帧开始前，先调用 ImGui_ImplOpenGL3_NewFrame() 和 ImGui_ImplGlfw_NewFrame() 来设置好 ImGui 的新帧状态，然后调用 ImGui::NewFrame() 来开始一个新的 ImGui 帧。
		//这三个函数的作用分别是：
		//ImGui_ImplOpenGL3_NewFrame()：准备 OpenGL3 后端的状态，确保它能正确处理 ImGui 的渲染命令。
		//ImGui_ImplGlfw_NewFrame()：准备 GLFW 后端的状态，处理输入事件等
		//ImGui::NewFrame()：开始一个新的 ImGui 帧，重置 ImGui 的内部状态，准备接受新的 UI 命令。
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if(currentTest){
			currentTest -> OnUpdate(0.0f);
			currentTest -> OnRender();
			ImGui::Begin("Test");
			//如果当前测试不是菜单，并且用户点击了 "<-" 按钮，就删除当前测试实例，并切换回菜单。
			if(currentTest != testMenu && ImGui::Button("<-")){
				delete currentTest;
				currentTest = testMenu;
			}
			currentTest -> OnImGuiRender();
			ImGui::End();
		}

/*
        // 5. 动画逻辑
        if(r > 1.0f || r < 0.0f){
            increment = -increment;
        }
        r += increment;
*/
		// 6. ImGui 渲染
		// ImGui::Render() 函数会结束当前的 ImGui 帧，并生成最终的绘制数据（ImDrawData）。这个数据包含了所有 ImGui 界面元素的顶点信息、索引信息和绘制命令。
		ImGui::Render();
		// glViewport(0, 0, display_w, display_h); // 如果窗口大小可变，应该在每帧更新视口大小
		// ImGui_ImplOpenGL3_RenderDrawData() 函数会根据 ImGui::GetDrawData() 获取到的绘制命令，使用 OpenGL API 来渲染 ImGui 的界面。
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // 6. 交换缓冲 & 事件
		// glfwSwapBuffers(window) 会交换前后缓冲区，显示我们刚刚渲染的内容。glfwPollEvents() 会处理所有的输入事件（键盘、鼠标等），并调用相应的回调函数。
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
	delete currentTest;
	if(currentTest != testMenu){
		delete testMenu;
	}
	// 7. 清理资源s
	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	glfwTerminate();
	return 0;
	}


/*
严格来说：**EBO 找不到 VBO。**流程是这样的（这是核心逻辑）：

你调用 glDrawElements(GL_TRIANGLES, 6, ...)
GPU 从 EBO 里依次取出索引：0、1、2、2、3、0
对每个索引 i（例如 i=2）：
GPU 去问 VAO：“顶点属性0怎么取？”
VAO 里记录着：属性0来自某个 VBO（比如 buffer），并且格式为：
size=2（x,y）
type=float
stride=2*sizeof(float)
offset=0
于是 GPU 计算地址：
base + offset + i * stride
读出第 i 个顶点的 2 个 float，作为 layout(location=0) in vec4 position 的输入（你 shader 里是 vec4，但你只提供 2 分量时，OpenGL 会补齐为 (x,y,0,1)）
所以：EBO 的索引 i → VAO 的顶点属性规则 → 去相应 VBO 按 stride/offset 取第 i 个顶点数据。
*/


/*
好问题！我们用一个非常形象的比喻来串联这段代码。

把整个 **OpenGL 渲染管线** 想象成一个 **“自动化工厂流水线”**。
你的 C++ 代码（CPU）是 **总指挥**，而显卡（GPU）是 **流水线工人**。

我们来看看你的代码是如何建立和指挥这条流水线的。

---

### 第一步：准备原料（Buffer - 缓冲区）
工厂要生产产品（图形），首先要有原材料（顶点数据）。

*   **VBO (Vertex Buffer Object) - 仓库里的原材料堆**
    *   **代码对应**：
        ```cpp
        float positionsp[] = { ... }; // 你的原始数据（坐标、纹理坐标）
        VertexBuffer vb(positionsp, sizeof(positionsp)); 
        ```
    *   **逻辑**：
        1.  你的数据原本在 **CPU 内存**（可以在 positionsp 数组里修改）。
        2.  `glBufferData` 的作用就是把这堆数据打包，**通过快递运送到 GPU 的显存**里。
        3.  **VBO 就是显存里的一块仓库**，里面堆满了毫无意义的二进制数字。显卡现在只知道这里有一堆数，但这堆数是坐标？还是颜色？它还不知道。

*   **EBO/IBO (Index Buffer Object) - 组装说明书**
    *   **代码对应**：
        ```cpp
        unsigned int indices[6] = { 0,1,2, 2,3,0 };
        IndexBuffer ib(indices, 6);
        ```
    *   **逻辑**：
        *   为了节省原料，我们不重复存点。比如画一个矩形需要 2 个三角形（6 个点），但其实只有 4 个独特的角。
        *   EBO 就是一张清单，告诉显卡：“喂，先拿第 0 号原料，再拿第 1 号，再拿第 2 号组成第一个三角形...”

---

### 第二步：定义加工规则（VAO - 顶点数组对象）
这部分是最抽象但最重要的。
显卡看着 VBO 里那一堆 float 数字（0.5, 0.5, 1.0...）一脸懵逼。它需要你告诉它怎么解读。

*   **VAO (Vertex Array Object) - 流水线配置单**
    *   **代码对应**：
        ```cpp
        VertexArray va;
        VertexBufferLayout layout;
        layout.Push<float>(2); // 告诉它：前两个数是位置
        layout.Push<float>(2); // 告诉它：后两个数是纹理坐标
        va.AddBuffer(vb, layout);
        ```
    *   **逻辑**：
        *   **VAO 是一个档案夹**。
        *   当你调用 `glVertexAttribPointer`（在你的 `AddBuffer` 里封装了）时，你是在写这个档案夹：
            *   “从 VBO 的起点开始读，每隔 4 个 float 是一组（Stride）”
            *   “这一组里，前 2 个 float 拿去给 Shader 里的 `position` 变量”
            *   “后 2 个 float 拿去给 Shader 里的 `texCoord` 变量”
        *   **为什么要 VAO？** 下次你要画这个物体时，只要把这个档案夹拿出来（`va.Bind()`），显卡瞬间就知道去哪个 VBO 取数据，以及怎么读数据，不用再重新罗嗦一遍规则了。

---

### 第三步：加工车间（Shader - 着色器）
现在原料有了，读取规则也有了，数据开始流入加工车间。这是唯一一段跑在 GPU 上的 **可编程代码**。

*   **Vertex Shader (顶点着色器) - 造型师**
    *   **逻辑**：
        *   它拿到 VAO 喂给它的顶点（Local Space 本地坐标）。
        *   你的 Shader 里会有 `gl_Position = u_MVP * position;`。
        *   它的工作是把这块泥巴（顶点）摆放到世界正确的位置，再拍扁到屏幕上（投影）。
    *   **代码对应**：
        ```cpp
        glm::mat4 mvp = proj * view * model; 
        shader.SetUniformMat4f("u_MVP", mvp);
        ```
        这里 CPU 负责计算好相机矩阵（MVP），传给 GPU 这个造型师。

*   **Fragment Shader (片段着色器) - 上色师**
    *   **逻辑**：
        *   三角形形状定好了，里面每个像素（Fragment）填什么颜色？
        *   它会根据你传的颜色 `u_Color` 或者 贴图 `u_Texture` 来决定当前这个像素点的最终颜色。
    *   **代码对应**：
        ```cpp
        Texture texture("res/Textures/C.png");
        texture.Bind(); // 把贴图送到上色师手边
        shader.SetUniform1i("u_Texture", 0); 
        ```

---

### 第四步：开机生产（Render Loop - 渲染循环）
配置工作做完了，现在机器开动了（`while` 循环）。

```cpp
while(!glfwWindowShouldClose(window)){
    renderer.Clear(); // 1. 清理上一帧的废料

    // 2. 准备阶段：把需要的档案夹、程序、贴图都摆到工位上
    shader.Bind();
    texture.Bind(); 
    
    // 3. 核心绘制指令（按下启动按钮）

      这行代码实际上在喊：
      "按照 VAO (va) 的规则，
       去读 IBO (ib) 指定的索引，
       用 Shader (shader) 的逻辑，
       给我画出三角形来！"
    renderer.Draw(va, ib, shader); 

    // 4. 显示产品
    glfwSwapBuffers(window); // 把画好的画展示给用户

*/

/*
### 总结一下这个抽象关系：

1.  **VBO**：一堆生肉（数据）。
2.  **VAO**：切肉的刀法和摆盘说明书（数据格式）。
3.  **Shader**：烹饪机器（逻辑程序）。
4.  **Uniforms (MVP, Texture)**：烹饪时的调料和参数（外部输入）。
5.  **DrawCall**：开始做菜！

**核心思想：OpenGL 是一个“状态机”。**
你所做的一切 `Bind()` 操作，都是在设置当前状态。
*   `va.Bind()` -> 设置当前读取规则。
*   `shader.Bind()` -> 设置当前使用的加工程序。
*   `Draw()` -> 基于**当前所有绑定的状态**执行一次绘制。

这就是为什么你的代码里会有那么多 `Bind` 和 `Unbind`，你是在不断切换流水线上的工具。
*/




