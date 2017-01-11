#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL.h>

#include <cstdio>

#include <sstream>
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

void render(GameObject *go, std::vector<Light*> lights, Camera camera);

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
	CubeObject* go = new CubeObject(mesh, cubeShader, transform, elapsedTime, deltaTime);
	go->SetupTextures();

	std::vector<Light*> lights;
	auto light = new Light(glm::vec3(3.0f, 1.0f, 2.0f), glm::vec3(0.2f), glm::vec3(0.5f), glm::vec3(1.0f));
	auto light2 = new Light(glm::vec3(-3.0f, 1.0f, -2.0f), glm::vec3(0.2f), glm::vec3(0.5f), glm::vec3(1.0f));
	lights.push_back(light);
	lights.push_back(light2);

	//TODO: Create LightObject class
	std::vector<CubeObject*> lightObjects;
	
	for (size_t i = 0; i < lights.size(); i++)
	{
		glm::mat4 lightTransform;
		lightTransform = glm::translate(glm::scale(lightTransform, glm::vec3(0.5f)), glm::vec3(lights[i]->position.x, 
																								lights[i]->position.y, 
																								lights[i]->position.z)); //Scale by 0.5 then translate to correct position
		auto lo = new CubeObject(mesh, lightShader, lightTransform, elapsedTime, deltaTime);
		lightObjects.push_back(lo);
	}

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
		render(go, lights, camera);

		for (size_t i = 0; i < lightObjects.size(); i++)
		{
			lightObjects[i]->Tick();
			render(lightObjects[i], lights, camera);
		}

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

void handle_movement(Camera& camera, float deltaTime) //TODO: use arrow keys to move objects
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
	if (keys[GLFW_KEY_Q])
	{
		//roll
	}
	if (keys[GLFW_KEY_E])
	{
		//roll
	}

	if (mouseMoved)
	{
		mouseMoved = false;
		camera.Rotate(pitch, yaw);
	}

	camera.Translate(translation);
}

//TODO: Support multiple lights and multiple types of lights
void render(GameObject *go, std::vector<Light*> lights, Camera camera) {
	auto& sp = go->shaderProgram;
	glUseProgram(sp.shaderProgram);

	GLint numLights = glGetUniformLocation(sp.shaderProgram, "numLights");
	glUniform1i(numLights, lights.size());

	for (size_t i = 0; i < lights.size(); i++)
	{
		auto glargl = [i](const char* member) //Lambda that creates a string to serve as an argument for glGetUniformLocation
		{
			std::stringstream ss;
			ss << "lights[" << i << "]." << member;
			auto str = ss.str();
			return ss.str();
		};

		#define glarg(uni) glargl(uni).c_str() //Using a macro for this might be a bad idea but it makes the glGetUniformLocation calls look nicer lol

		//TODO: don't do this every frame
		//Set light properties
		GLint lightPositionLoc = glGetUniformLocation(sp.shaderProgram, glarg("position"));
		GLint lightAmbientLoc = glGetUniformLocation(sp.shaderProgram, glarg("ambient"));
		GLint lightDiffuseLoc = glGetUniformLocation(sp.shaderProgram, glarg("diffuse"));
		GLint lightSpecularLoc = glGetUniformLocation(sp.shaderProgram, glarg("specular"));

		#undef glarg

		glUniform3f(lightPositionLoc, lights[i]->position.x, lights[i]->position.y, lights[i]->position.z);
		glUniform3f(lightAmbientLoc, lights[i]->ambient.r, lights[i]->ambient.g, lights[i]->ambient.b);
		glUniform3f(lightDiffuseLoc, lights[i]->diffuse.r, lights[i]->diffuse.g, lights[i]->diffuse.b);
		glUniform3f(lightSpecularLoc, lights[i]->specular.r, lights[i]->specular.g, lights[i]->specular.b);
	}

	go->Draw(camera);
}