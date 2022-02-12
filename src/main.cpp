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

/*task 对函数进行封装，以此为参考，接口和类名可适当调整，
并将封装后的代码替换正文*/
// 创建shader  将83-103封装
GLuint createShader(GLuint type, const char* src) { return 0; }
GLuint createProgram(const std::vector<GLuint>& shaders) { return 0; }
void deleteShaders(const std::vector<GLuint>& shaders) {}
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
VertexHandle createHandle(const VertexDataCreateInfo& vd) { return VertexHandle(); }
void deleteHandle(const VertexHandle& hd) {}


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

	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;
	
	//顶点着色器
	const char* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"  //位置属性0
		"layout (location = 1) in vec3 aColor;\n" //颜色属性1
		"uniform float offset;"
		"out vec3 color;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x + offset,aPos.y,aPos.z,1.0f);\n"
		"   color = aColor;\n"
		"}\0";
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShaderId);
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	
	//片段着色器
	const char* fragmentShaderSource = "#version 330 core\n"
		"uniform vec4 ourColor;"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"	FragColor = ourColor;\n"  //uniform变量在外面更新
		"}\n";
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShaderId);
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	
	//黄色片段着色器
	const char* fragmentShaderYellowSrc = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec3 color;\n"
		"void main()\n"
		"{\n"
		"	FragColor = vec4(color,1.0f);\n"
		"}\n";
	GLuint fragmentShaderYellow = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderYellow,1,&fragmentShaderYellowSrc,NULL);
	glCompileShader(fragmentShaderYellow);
	glGetShaderiv(fragmentShaderYellow, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderYellow, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	
	//两个着色器全部编译完毕后连接到shader程序里
	GLuint shaderPragram = glCreateProgram();
	glAttachShader(shaderPragram, vertexShaderId);
	glAttachShader(shaderPragram, fragmentShaderId);
	glLinkProgram(shaderPragram);
	glGetProgramiv(shaderPragram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderPragram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	
	GLuint shaderPragramYellow = glCreateProgram();
	glAttachShader(shaderPragramYellow,vertexShaderId);  
	glAttachShader(shaderPragramYellow, fragmentShaderYellow);
	glLinkProgram(shaderPragramYellow);
	glGetProgramiv(shaderPragramYellow, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderPragramYellow, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	
	//link之后删除之前的两个着色器对象
	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);
	glDeleteShader(fragmentShaderYellow);
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

	GLuint VAO; //指定VBO中的数据如何解析
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	GLuint VBO1; //第一个三角形
	glGenBuffers(1, &VBO1); //创建新的buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle, GL_STATIC_DRAW); //设置buffer的大小
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); //链接VAO与VBO
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	GLuint VAO2;
	glGenVertexArrays(1,&VAO2);
	glBindVertexArray(VAO2);
	
	GLuint VBO2;  //第二个三角形,vbo2中现在包含3个顶点的颜色
	glGenBuffers(1,&VBO2);
	glBindBuffer(GL_ARRAY_BUFFER,VBO2);
	glBufferData(GL_ARRAY_BUFFER,sizeof(secondTriangle),secondTriangle,GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //解析位置信息
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE, 6 * sizeof(float), (void*)0);  //解析颜色信息
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	float position = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		//在每次循环开始的地方处理输入事件
		processInput(window);
		//每次重回背景
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(shaderPragram);
		
		//将第一个三角形的颜色随着时间改变
		float time = glfwGetTime();
		float greenVal = sin(time) / 2.0f + 0.5f;
		int ourColorUniform = glGetUniformLocation(shaderPragram,"ourColor");
		glUniform4f(ourColorUniform,0.0f,greenVal,0.0f,1.0f);

		int offsetUniform = glGetUniformLocation(shaderPragram,"offset");
		glUniform1f(offsetUniform,position);
		position += 0.0001f;
		
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
		
		glUseProgram(shaderPragramYellow);
		glBindVertexArray(VAO2);
		glDrawArrays(GL_TRIANGLES,0,3);
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