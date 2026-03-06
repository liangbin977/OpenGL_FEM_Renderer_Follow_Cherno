#include <glad/glad.h>  // 必须放在第一行！它包含了 OpenGL 的头文件。如果放在 GLFW 后面，编译器会报错。
#include "Renderer.h"
#include "Shader.h"
#include <iostream>    // 用于输出错误信息到控制台。
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
Shader::Shader(const std::string& filepath)
    :m_FilePath(filepath), m_RendererID(0)
{
    ShaderProgramSource source = ParseShader(filepath);
    m_RendererID = Shader::CreateShader(source.VertexSource, source.FragmentSource, source.ComputeSource);
};
Shader::~Shader(){
    GLCall(glDeleteProgram(m_RendererID));
};
void Shader::SetUniform1i(const std::string& name, int value){
    GLCall(glUniform1i(GetUniformLocation(name), value));
};
void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3){
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
};
void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix){
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
};
void Shader::SetUniform1f(const std::string& name, float value){
	GLCall(glUniform1f(GetUniformLocation(name), value));
}
void Shader::SetUniform2f(const std::string& name, float v0, float v1){
	GLCall(glUniform2f(GetUniformLocation(name), v0, v1));
}
//GLSL 中没有 bool 类型的 Uniform，通常我们会用 int 来代替，0 表示 false，非 0 表示 true。
void Shader::SetUniformBool(const std::string& name, bool value){
	GLCall(glUniform1i(GetUniformLocation(name), (int)value));
}
void Shader::Bind()const{
    GLCall(glUseProgram(m_RendererID));
};
void Shader::Unbind()const{
    GLCall(glUseProgram(0));
};

ShaderProgramSource Shader::ParseShader(const std::string& filepath){
    enum class ShaderType{
		NONE = -1, VERTEX = 0, FRAGMENT = 1, COMPUTE = 2
	};
    std::ifstream stream(filepath);
    if (!stream.is_open()) {
        std::cout << "[Error] Failed to open shader file: " << filepath << std::endl;
        std::cout << "        Please check working directory!" << std::endl;
        return { "", "", "" };
    }    
	std::string line;
	//使用字符串流来存储顶点着色器和片段着色器的源代码
	std::stringstream ss[3];
	ShaderType type = ShaderType::NONE;
	while(getline(stream, line)){
		//不使用string.end()来判断是否包含子串，因为它会返回一个迭代器，而不是一个布尔值。正确的做法是使用string::npos来判断是否找到子串。
		if(line.find("#shader")!=std::string::npos){
			if(line.find("vertex")!=std::string::npos)
				type = 	ShaderType::VERTEX;
			else if(line.find("fragment")!=std::string::npos)
				type = ShaderType::FRAGMENT;
			else if(line.find("compute")!=std::string::npos)
				type = ShaderType::COMPUTE;
		}
		else {
            if(type != ShaderType::NONE)
			    ss[(int)type] << line << '\n';
		}

	}
	//.str() 方法可以将字符串流中的内容转换为一个 std::string 对象。
    return {ss[0].str(), ss[1].str(), ss[2].str()};
};

unsigned int Shader::CompileShader(unsigned int type, const std::string& source){
	//创建一个着色器对象，type 可以是 GL_VERTEX_SHADER 或 GL_FRAGMENT_SHADER
	unsigned int id = glCreateShader(type);
	// c_string 提供了一个 c_str() 方法，可以返回一个指向字符串数据的常量字符指针（const char*）。
	const char* src = source.c_str();
	//glShaderSource — Replaces the source code in a shader object
	//作用是：把刚才转换好的源代码src“塞”进刚才创建的着色器对象里。
	//第一个参数是着色器对象的 ID，
	//第二个参数是传入的字符串数量，
	//第三个参数是指向字符串数组的指针，
	//第四个参数是每个字符串的长度（传 nullptr 表示字符串以 null 结尾）。
	//因为 OpenGL 允许你传入一个字符串数组（比如代码分散在好几个部分），所以它要求传入的是“字符串指针类型的数组
	glShaderSource(id, 1, &src, nullptr);
	// 编译着色器对象
	glCompileShader(id);
	int result;
	//检查编译是否成功
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if(result == GL_FALSE){
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		// char* message = new char[length];
		//alloc返回void指针，指向分配的内存块的起始地址
		char* message = (char*)alloca(length * sizeof(char));
		//获取错误信息日志
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile "
		          << (type == GL_VERTEX_SHADER ? "vertex" : (type == GL_FRAGMENT_SHADER ? "fragment" : "compute"))
		          << " shader!" << std::endl;
		std::cout << message << std::endl;
		return 0;
	};
	return id;
};

unsigned int Shader::CreateShader(const std::string& VertexShader, const std::string& FragmentShader, const std::string& ComputeShader){
    // 创建一个空的着色器程序对象
	unsigned int program = glCreateProgram();
	if(!VertexShader.empty()){
		unsigned int vs = CompileShader(GL_VERTEX_SHADER, VertexShader);
		if(vs != 0){
			glAttachShader(program, vs);
			glDeleteShader(vs); // 删除顶点着色器对象，因为它已经被链接到程序对象上了，删除不会影响程序的运行
		}
		else{
			std::cout << "Error: Failed to compile vertex shader!" << std::endl;
		}
	}
	if(!FragmentShader.empty()){
		unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, FragmentShader);
		if(fs != 0){
			glAttachShader(program, fs);
			glDeleteShader(fs); // 删除片段着色器对象，因为它已经被链接到程序对象上了，删除不会影响程序的运行
		}
		else{
			std::cout << "Error: Failed to compile fragment shader!" << std::endl;
		}
	}
	if(!ComputeShader.empty()){
		unsigned int cs = CompileShader(GL_COMPUTE_SHADER, ComputeShader);
		if(cs != 0){
			glAttachShader(program, cs);
			glDeleteShader(cs); // 删除计算着色器对象，因为它已经被链接到程序对象上了，删除不会影响程序的运行
		}
		else{
			std::cout << "Error: Failed to compile compute shader!" << std::endl;
		}
	}

	glLinkProgram(program); // 链接程序对象
	glValidateProgram(program); // 验证程序对象

    int isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE) {
		int maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		char* infoLog = (char*)alloca(maxLength * sizeof(char));
		glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);
		std::cout << "Failed to link shader program!" << std::endl;
		std::cout << infoLog << std::endl;
		glDeleteProgram(program);
		return 0;
	}
	return program;
};

int Shader::GetUniformLocation(const std::string& name){
    if(m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];
    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if(location == -1){
        std::cout << "Warning: uniform " << name << " doesn't exist!" << std::endl;
    }
    m_UniformLocationCache[name] = location;
    return location;
};
unsigned int Shader::GetRendererID() const {
	return m_RendererID;
};