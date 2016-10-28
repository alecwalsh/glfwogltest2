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

#include "ShaderProgram.h"
#include "GameObject.h"
#include "CubeObject.h"
#include "Light.h"
#include "Camera.h"
//TODO: clean up duplicate includes

//Prototypes for input handling callbacks
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void handle_movement(Camera& camera, float deltaTime);

//TODO: avoid globals
// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

float lastX = WIDTH / 2.0;
float lastY = HEIGHT / 2.0;

double yaw = -90.0f;
double pitch = 0.0f;

bool keys[1024];

int main(int argc, char* argv[]) {
	auto t_start = std::chrono::high_resolution_clock::now();
	auto t_prev = t_start;
	float elapsedTime = 0.0f;
	float deltaTime = 0.0f;

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", nullptr, nullptr); // Windowed

	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
	auto cubeShader = ShaderProgram("shaders/vert_cube.glsl", "shaders/frag_cube.glsl");
	auto lightShader = ShaderProgram("shaders/vert_light.glsl", "shaders/frag_light.glsl");

	//std::vector<GLuint> elements = {
	//	0, 1, 2, 2, 3, 0,
	//	4, 5, 6, 6, 7, 4,
	//	8, 9, 10, 10, 11, 8,
	//	12, 13, 14, 14, 15, 12,
	//	16, 17, 18, 18, 19, 16,
	//	20, 21, 22, 22, 23, 20
	//};

	auto mesh = Mesh("data/cube.txt");

	auto camera = Camera(
		glm::vec3(2.0f, 2.0f, 2.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f) //y-axis is up
	);


	//Sets pitch and yaw based on the cameraFront vector;  this prevents the camera from jumping when moving the mouse for the first time
	auto& cf = camera.cameraFront;

	pitch = glm::degrees(asin(cf.y));
	yaw = glm::degrees(acos(cf.x / cos(asin(cf.y))));

	if (cf.z < 0) {
		yaw = -yaw;
	}

	//Creates a CubeObject
	glm::mat4 transform;
	//transform = glm::rotate(transform, glm::radians(0.0f), glm::vec3(1, 0, 0));
	CubeObject* go = new CubeObject(mesh, cubeShader, transform, elapsedTime, deltaTime);
	//TODO: The following line causes a black screen, find out why
	Light* light = new Light(mesh, lightShader, transform, elapsedTime, deltaTime);


	//main loop
	while (!glfwWindowShouldClose(window))
	{
		auto t_now = std::chrono::high_resolution_clock::now();

		deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_prev).count();
		t_prev = t_now;

		elapsedTime = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();

		glfwPollEvents();
		handle_movement(camera, deltaTime);


		// Clear the screen to black
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		go->Tick();
		go->Draw(camera);

		//Swap buffers
		glfwSwapBuffers(window);
	}

	delete go;

	//std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "bye\n";

	glfwTerminate();

	return 0;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (action == GLFW_PRESS)
	{
		keys[key] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		keys[key] = false;
	}

	// When a user presses the escape key, we set the WindowShouldClose property to true, closing the application
	if (keys[GLFW_KEY_ESCAPE])
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}


bool mouseMoved = false;
bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	mouseMoved = true;
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xSensitivity = 0.2f;
	float ySensitivity = 0.2f;

	auto deltaX = lastX - xpos;
	auto deltaY = lastY - ypos;

	lastX = xpos;
	lastY = ypos;


	yaw -= deltaX * xSensitivity;
	pitch += deltaY *  ySensitivity;

	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	if (pitch < -89.0f) {
		pitch = -89.0f;
	}
}

void handle_movement(Camera& camera, float deltaTime)
{
	glm::mat4 translation;

	//Calculates vectors from the perspective of the camera
	//This allows the camera to work no matter how it is moved and rotated
	glm::vec3 rightVector = glm::normalize(glm::cross(camera.cameraFront, camera.up));
	glm::vec3 leftVector = -rightVector;
	glm::vec3 frontVector = camera.cameraFront;
	glm::vec3 backVector = -frontVector;
	glm::vec3 upVector = camera.up;
	glm::vec3 downVector = -upVector;

	float velocity = 2.5f;

	//TODO: Get key bindings from files
	//TODO: Figure out how to use control key

	//Changes the cameras location in response to keypresses
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		translation = glm::translate(translation, velocity * deltaTime * leftVector);
	}
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		translation = glm::translate(translation, velocity * deltaTime * rightVector);
	}

	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		translation = glm::translate(translation, velocity * deltaTime * frontVector);
	}
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		translation = glm::translate(translation, velocity * deltaTime * backVector);
	}

	if (keys[GLFW_KEY_SPACE])
	{
		translation = glm::translate(translation, velocity * deltaTime * upVector);
	}
	if (keys[GLFW_KEY_C])
	{
		translation = glm::translate(translation, velocity * deltaTime * downVector);
	}

	if (mouseMoved)
	{
		mouseMoved = false;
		camera.Rotate(pitch, yaw);
	}

	camera.Translate(translation);
}