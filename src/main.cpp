#ifdef OPENGL_TEST
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;
//设置窗口的宽和高
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
//响应键盘输入事件
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}
//设置窗口大小回调
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
//主函数
int main()
{
	glfwInit();
	//声明版本与核心,opengl3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//创建窗口并设置其大小，名称，与检测是否创建成功
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); 
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//顶点着色器
	const char* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";
	unsigned int vertexShaderId = 0;  //顶点着色器ID
	vertexShaderId = glCreateShader(GL_VERTEX_SHADER); //创建shader
	glShaderSource(vertexShaderId, 1, &vertexShaderSource, NULL);  //设置shader代码
	glCompileShader(vertexShaderId);//编译shader

	//片段着色器
	const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\n";
	unsigned int fragmentShaderId = 0;
	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShaderId);

	//两个着色器全部编译完毕后连接到shader程序里
	unsigned int shaderPragram = 0;
	shaderPragram = glCreateProgram();
	glAttachShader(shaderPragram, vertexShaderId);
	glAttachShader(shaderPragram, fragmentShaderId);
	glLinkProgram(shaderPragram);
	//link之后删除之前的两个着色器对象
	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

	//定义一个三角形
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};
	unsigned int VBO; //设置顶点缓冲区
	glGenBuffers(1, &VBO); //创建新的buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //设置buffer的大小

	unsigned int VAO; //指定VBO中的数据如何解析
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	while (!glfwWindowShouldClose(window))
	{
		//在每次循环开始的地方处理输入事件
		processInput(window);
		//每次重回背景
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(shaderPragram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

#endif


#ifdef VULKAN_TEST
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

	uint32_t extensionCount = 0;

	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::cout << extensionCount << " extensions supported." << std::endl;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();

	return 0;
}

#endif