#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#include <glm/glm.hpp>

#include <iostream>
//#include <thread>
#include <chrono>

#include "Object.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);


// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Shader sources
const GLchar* vertexSource =
"#version 150 core\n"
"in vec2 position;"
"in vec3 color;"
"out vec3 Color;"
"void main() {"
"	Color = color;"
"   gl_Position = vec4(position, 0.0, 1.0);"
"}";

const GLchar* fragmentSource =
"#version 150 core\n"
"in vec3 Color;"
"out vec4 outColor;"
"void main() {"
"   outColor = vec4(Color, 1.0);"
"}";

int main(int argc, char** argv) {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", nullptr, nullptr); // Windowed

	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);


	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Error initializing GLEW";
		glfwTerminate();
		return -1;
	}

	GLuint elements[] = {
		0, 1, 2,
		1, 2, 3
	};

	float vertices[] = {
		-0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 1.0f, 1.0f, 1.0f
	};


	// Create and compile the vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	// Create and compile the fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	// Link the vertex and fragment shader into a shader program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);

	auto& object1 = Object(vertices, 4, elements, 6, shaderProgram);
	auto& object2 = Object(vertices, 4, elements, 3, shaderProgram);

	//main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();


		// Clear the screen to black
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		object1.Draw();
		object2.Draw();

		//Swap buffers
		glfwSwapBuffers(window);
	}


	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);


	//std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "hi";

	glfwTerminate();

	return 0;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	// When a user presses the escape key, we set the WindowShouldClose property to true, closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}