#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL.h>

#include <iostream>
//#include <thread>
#include <vector>
#include <chrono>

#include "Object.h"
#include "ShaderProgram.h"
#include "GameObject.h"

//TODO: clean up duplicate includes

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

int main(int argc, char** argv) {
	auto t_start = std::chrono::high_resolution_clock::now();
	
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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//compile and link shaders
	auto sp = ShaderProgram("shaders/vert1.glsl", "shaders/frag1.glsl");
	GLuint shaderProgram = sp.shaderProgram;


	GLuint elements[] = {
		0, 1, 2,
		1, 2, 3
	};

	//float vertices[] = {
	//	//Position          Color             Texcoords
	//	-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	//	-0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
	//	0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	//	0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f
	//};

	std::vector<GLfloat> vertices({
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f
	});

	auto mesh = Mesh(vertices);
	auto go = GameObject(mesh, sp, glm::mat4());

	auto& object1 = Object(vertices.data(), 36, elements, 36, shaderProgram);
	//auto& object2 = Object(vertices, 4, elements, 3, shaderProgram);

	//TODO: Move all uniform stuff into GameObject
	//TODO: Add global way to get time / elapsed time / delta time
	GLint uniTime = glGetUniformLocation(shaderProgram, "time");

	GLint uniModel = glGetUniformLocation(shaderProgram, "model");

	//main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		auto t_now = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();

		glm::mat4 model;
		glm::mat4 rotation;
		glm::mat4 translation;
		glm::mat4 scaling;

		rotation = glm::rotate(
			rotation,
			time * glm::radians(180.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
			);
		translation = glm::translate(translation, glm::vec3(0.0f, 0.0f, 0.0f));
		scaling = glm::scale(scaling, glm::vec3(1.0f, 1.0f, 1.0f));

		model = translation * rotation * scaling;
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

		bool ascending = ((int)time / 5) % 2 == 0;

		time = time / 5;
		if (ascending){
			glUniform1f(uniTime, time - floor(time));
		}
		else {
			glUniform1f(uniTime, 1 - (time - floor(time)));
		}
		

		// Clear the screen to black
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		object1.Draw();
		//object2.Draw();

		//Swap buffers
		glfwSwapBuffers(window);
	}


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