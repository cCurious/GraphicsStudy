#ifdef OPENGL_TEST
#include <vector>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
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

GLuint LoadTexture(std::string texturePath)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	int imageWidth = 0;
	int imageHeigth = 0;
	int channels = 0;
	unsigned char * image = stbi_load(texturePath.c_str(),&imageWidth,&imageHeigth,&channels,0);
	if(image != nullptr)
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeigth, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);//设置纹理环绕，超过纹理点的部分将会被镜像复制
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_image_free(image);
		return textureID;
	}
	return 0;
}

/*task 对函数进行封装，以此为参考，接口和类可适当调整，
并将封装后的代码替换正文*/
// 创建shader  将83-103封装
GLuint createShader(GLuint type, const char* src)
{ 
	GLuint shaderID = glCreateShader(type);
	glShaderSource(shaderID,1,&src,nullptr);
	glCompileShader(shaderID);
	int success;
	char infoLog[512];
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		return 0;
	}
	return shaderID;
}

GLuint createProgram(const std::vector<GLuint>& shaders)
{ 
	GLuint shaderProgram = glCreateProgram();
	for (auto iter : shaders)
	{
		glAttachShader(shaderProgram,iter);
	}
	glLinkProgram(shaderProgram);
	int success = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	char infoLog[512] = {0};
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		return 0;
	}
	return shaderProgram;
}

void deleteShaders(const std::vector<GLuint>& shaders)
{
	for (auto iter : shaders)
	{
		glDeleteShader(iter);
	}
}

struct VertexLayoutElement {
    GLuint layoutIndex; //对应glsl中location的值
    GLint size; //基础数据类型个数
    GLenum type;
    bool normalized; //是否归一化
    GLsizei stride; //该元素在内存中相邻两次出现之间的间隔
    unsigned int offset; //单次出现相对起始位置的偏移量
    int attributeDivisor = 0; //added for instanced render
};
using VertexBufferLayout = std::vector<VertexLayoutElement>;
struct VertexHandle {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};
struct VertexDataCreateInfo {
    GLenum bufType; //GL_ARRAY_BUFFER
    void* vertice = nullptr;
    GLint vertexCount = 0;
    GLint vertexSiz = 0;
    void* indice = nullptr;
    GLint indiceCount = 0;
    GLint indiceSiz = 0;
    GLenum drawUsage; //GL_STATIC_DRAW
    VertexBufferLayout layout;
};

//封装vao创建那一段
VertexHandle createHandle(const VertexDataCreateInfo& vd)
{
	VertexHandle handle;
	glGenVertexArrays(1,&handle.vao);
	glGenBuffers(1, &handle.vbo);
	glGenBuffers(1,&handle.ebo);

	glBindVertexArray(handle.vao);
	glBindBuffer(vd.bufType,handle.vbo); //初始化vbo
	glBufferData(vd.bufType,vd.vertexSiz,vd.vertice,vd.drawUsage); //buffer数据传输进显卡之后，就可以初始化顶点信息了
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle.ebo);//初始化ebo
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vd.indiceSiz, vd.indice, vd.drawUsage); //buffer数据传输进显卡之后，就可以初始化顶点信息了
	for (auto iter : vd.layout)
	{
		//解析顶点数据
		glVertexAttribPointer(iter.layoutIndex,iter.size,iter.type,iter.normalized,iter.stride,0);
		glEnableVertexAttribArray(iter.layoutIndex);
	}
	return handle; 
}
void deleteHandle(const VertexHandle& hd)
{
	glDeleteBuffers(1, &hd.vao);
	glDeleteBuffers(1,&hd.ebo);
	glDeleteBuffers(1,&hd.vbo);
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
		"layout (location = 0) in vec3 aPos;\n"  //位置属性0
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos,1.0f);\n"
		"}\0";
	//黄色片段着色器
	const char* fragmentShaderYellowSrc = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"	FragColor = vec4(1.0f,1.0f,0.0f,1.0f);\n"
		"}\n";

	float firstTriangle[] = {		
		-0.9f, -0.5f,  0.0f,  // left 
        - 0.0f, -0.5f, 0.0f,  // right
        - 0.45f, 0.5f, 0.0f // top 
	};
	float secondTriangle[] = {
		//顶点				//位置
		 0.0f, -0.5f, 0.0f,1.0f, 0.0f, 0.0f,  // left
		 0.9f, -0.5f, 0.0f,0.0f, 1.0f, 0.0f,  // right
         0.45f, 0.5f, 0.0f,0.0f, 0.0f, 1.0f  // top 
	};
	
	std::vector<GLuint> shaders;
	GLuint vertexShader = createShader(GL_VERTEX_SHADER,vertexShaderSource);
	GLuint yellowFragmentShader = createShader(GL_FRAGMENT_SHADER,fragmentShaderYellowSrc);
	shaders.push_back(vertexShader);
	shaders.push_back(yellowFragmentShader);
	GLuint shaderPragram = createProgram(shaders);
	deleteShaders(shaders);

	VertexDataCreateInfo dataInfo;
	dataInfo.bufType = GL_ARRAY_BUFFER;
	dataInfo.vertice = firstTriangle;
	dataInfo.vertexSiz = sizeof(firstTriangle);
	dataInfo.vertexCount = sizeof(firstTriangle)/sizeof(float);
	dataInfo.drawUsage = GL_STATIC_DRAW;
	VertexLayoutElement vertexElement;
	vertexElement.layoutIndex = 0;
	vertexElement.normalized = false;
	vertexElement.offset = 0;
	vertexElement.size = 3;
	vertexElement.stride = 3 * sizeof(float);
	vertexElement.type = GL_FLOAT;

	/*VertexLayoutElement colorElement;
	vertexElement.layoutIndex = 1;
	vertexElement.normalized = false;
	vertexElement.offset = 3;
	vertexElement.size = 3;
	vertexElement.stride = 3;
	vertexElement.type = GL_FLOAT;*/

	dataInfo.layout.push_back(std::move(vertexElement));
	//dataInfo.layout.push_back(std::move(colorElement));
	VertexHandle handle = createHandle(dataInfo);
	while (!glfwWindowShouldClose(window))
	{
		//在每次循环开始的地方处理输入事件
		processInput(window);
		//每次重回背景
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(handle.vao);
		glUseProgram(shaderPragram);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

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