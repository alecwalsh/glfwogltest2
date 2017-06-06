#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <SOIL/SOIL.h>

#include <cstdio>
#include <unistd.h>

#include <sstream>
#include <iostream>
//#include <thread>
#include <vector>
#include <chrono>
#include <memory>

#include "ShaderProgram.h"
#include "GameObject.h"
#include "CubeObject.h"
#include "PointLight.h"
#include "DirLight.h" //TODO:  shouldn't have to include both types of light
#include "Camera.h"
#include "TextureManager.h"
//TODO: clean up duplicate includes


//TODO: move this to separate file
struct global_values
{
    GLuint WIDTH, HEIGHT;
    float lastX, lastY;
    double yaw, pitch;
    bool hasResized = false;
};

//Prototypes for input handling callbacks
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void handle_movement(global_values* gv, Camera& camera, float deltaTime);

void render(GameObject& go, std::vector<std::unique_ptr<PointLight>>& pointLights, std::vector<DirLight*> dirLights, Camera camera);

//TODO: avoid globals, use glfwSetWindowUserPointer
bool keys[1024];

int main(int argc, char* argv[]) {
    chdir(".."); //Data files and shaders are in parent directory
	auto t_start = std::chrono::high_resolution_clock::now();
	auto t_prev = t_start;
	float elapsedTime = 0.0f;
	float deltaTime = 0.0f;


    
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 8);

	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr); // Windowed
    
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    //Set global state and call glfwSetWindowUserPointer to make it accessable to callbacks
    global_values gs;
    
    gs.hasResized = false;
    gs.WIDTH = 800;
    gs.HEIGHT = 600;
    gs.lastX = gs.WIDTH / 2;
    gs.lastY = gs.HEIGHT / 2;
    
    glfwSetWindowUserPointer(window, &gs);
    
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Error initializing GLEW";
		glfwTerminate();
		return -1;
	}

	
    glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//compile and link shaders
	auto cubeShader = ShaderProgram("shaders/vert_cube.glsl", "shaders/frag_cube.glsl");
	auto lightShader = ShaderProgram("shaders/vert_light.glsl", "shaders/frag_light.glsl");

    //TODO: use .blend files
    auto mesh = Mesh("data/cube_irreg.fbx");
    auto lightMesh = Mesh("data/cube.fbx");

	//Create textures
	TextureManager texman;
	texman.AddTexture("data/normalmaptest1.png");
	texman.AddTexture("data/container2.png");
	texman.AddTexture("data/sample2.png");
	texman.AddTexture("data/container2_specular.png");

	auto camera = Camera(
		glm::vec3(2.0f, 2.0f, 2.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f) //y-axis is up
	);


	//Sets pitch and yaw based on the cameraFront vector;  this prevents the camera from jumping when moving the mouse for the first time
    //This is just the inverse of the code in Camera::Rotate
	auto& cf = camera.cameraFront;

	gs.pitch = glm::degrees(asin(cf.y));
	gs.yaw = glm::degrees(acos(cf.x / cos(asin(cf.y))));

	if (cf.z < 0) {
		gs.yaw = -gs.yaw;
	}

	//Creates a CubeObject
	glm::mat4 transform;
	auto go = std::make_unique<CubeObject>(mesh, cubeShader, transform, elapsedTime, deltaTime, texman);
	go->SetupTextures();

    //TODO: use std::vector<std::unique_ptr<Light>>
    std::vector<std::unique_ptr<PointLight>> pointLights;
	auto pointLight = std::make_unique<PointLight>(glm::vec3(3.0f, 1.0f, 2.0f), glm::vec3(0.5f), glm::vec3(1.0f));
	auto pointLight2 = std::make_unique<PointLight>(glm::vec3(-6.0f, 1.0f, -2.0f), glm::vec3(0.5f), glm::vec3(1.0f));
	pointLights.push_back(std::move(pointLight));
	pointLights.push_back(std::move(pointLight2));
    
    std::vector<DirLight*> dirLights;
    auto dirLight = new DirLight(glm::vec3(0.0f, 1.0f, -1.0f), glm::vec3(0.5f), glm::vec3(0.25f));
    dirLights.push_back(dirLight);

	//TODO: Create LightObject class
	//The white cubes that represent lights
	std::vector<std::unique_ptr<GameObject>> lightObjects;
	
	for (size_t i = 0; i < pointLights.size(); i++)
	{
		glm::mat4 lightTransform;
		lightTransform = glm::translate(glm::scale(lightTransform, glm::vec3(0.5f)), glm::vec3(pointLights[i]->position.x, 
																								pointLights[i]->position.y, 
																								pointLights[i]->position.z)); //Scale by 0.5 then translate to correct position
		auto lo = std::make_unique<CubeObject>(lightMesh, lightShader, lightTransform, elapsedTime, deltaTime, texman);
		lightObjects.push_back(std::move(lo));
	}

	//main loop
	while (!glfwWindowShouldClose(window))
	{
		auto t_now = std::chrono::high_resolution_clock::now();

		deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_prev).count();
		t_prev = t_now;

		elapsedTime = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
        
        if(gs.hasResized)
        {
            
            glUseProgram(go->shaderProgram.shaderProgram);
            glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float) gs.WIDTH / gs.HEIGHT, 1.0f, 10.0f);
            GLint uniProj = glGetUniformLocation(go->shaderProgram.shaderProgram, "proj");
            glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
            
            glUseProgram(lightObjects[1]->shaderProgram.shaderProgram);
            
            GLint uniProj2 = glGetUniformLocation(lightObjects[1]->shaderProgram.shaderProgram, "proj");
            glUniformMatrix4fv(uniProj2, 1, GL_FALSE, glm::value_ptr(proj));
        }

		glfwPollEvents();
		handle_movement(&gs, camera, deltaTime);


		// Clear the screen to black
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		go->Tick();
		render(*go, pointLights, dirLights, camera);

		for (size_t i = 0; i < lightObjects.size(); i++)
		{
			lightObjects[i]->Tick();
			render(*lightObjects[i], pointLights, dirLights, camera);
		}

		//Swap buffers
		glfwSwapBuffers(window);
	}

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
    float& lastX = static_cast<global_values*>(glfwGetWindowUserPointer(window))->lastX;
    float& lastY = static_cast<global_values*>(glfwGetWindowUserPointer(window))->lastY;
    
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

    auto& yaw = static_cast<global_values*>(glfwGetWindowUserPointer(window))->yaw;
    auto& pitch = static_cast<global_values*>(glfwGetWindowUserPointer(window))->pitch;
	yaw -= deltaX * xSensitivity;
	pitch += deltaY *  ySensitivity;

	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	if (pitch < -89.0f) {
		pitch = -89.0f;
	}
}

void handle_movement(global_values* gv, Camera& camera, float deltaTime) //TODO: use arrow keys to move objects
{
    auto& yaw = gv->yaw;
    auto& pitch = gv->pitch;
    
    
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
void render(GameObject& go, std::vector<std::unique_ptr<PointLight>>& pointLights, std::vector<DirLight*> dirLights, Camera camera) {
	auto& sp = go.shaderProgram;
	glUseProgram(sp.shaderProgram);

	GLint numPointLights = glGetUniformLocation(sp.shaderProgram, "numPointLights");
	glUniform1i(numPointLights, pointLights.size());
    
    GLint numDirLights = glGetUniformLocation(sp.shaderProgram, "numDirLights");
	glUniform1i(numDirLights, dirLights.size());

    //TODO: lots of duplicated code
	for (size_t i = 0; i < pointLights.size(); i++)
	{
		auto glargl = [i](const char* member) //Lambda that creates a string to serve as an argument for glGetUniformLocation
		{
			std::stringstream ss;
			ss << "pointLights[" << i << "]." << member;
			auto str = ss.str();
			return ss.str();
		};

		#define glarg(uni) glargl(uni).c_str() //Using a macro for this might be a bad idea but it makes the glGetUniformLocation calls look nicer lol

		//TODO: don't do this every frame
		//Set light properties
		GLint lightPositionLoc = glGetUniformLocation(sp.shaderProgram, glarg("position"));
		GLint lightDiffuseLoc = glGetUniformLocation(sp.shaderProgram, glarg("diffuse"));
		GLint lightSpecularLoc = glGetUniformLocation(sp.shaderProgram, glarg("specular"));

		#undef glarg

		glUniform3f(lightPositionLoc, pointLights[i]->position.x, pointLights[i]->position.y, pointLights[i]->position.z);
		glUniform3f(lightDiffuseLoc, pointLights[i]->diffuse.r, pointLights[i]->diffuse.g, pointLights[i]->diffuse.b);
		glUniform3f(lightSpecularLoc, pointLights[i]->specular.r, pointLights[i]->specular.g, pointLights[i]->specular.b);
	}
	
    for (size_t i = 0; i < dirLights.size(); i++)
	{
		auto glargl = [i](const char* member) //Lambda that creates a string to serve as an argument for glGetUniformLocation
		{
			std::stringstream ss;
			ss << "dirLights[" << i << "]." << member;
			auto str = ss.str();
			return ss.str();
		};

		#define glarg(uni) glargl(uni).c_str() //Using a macro for this might be a bad idea but it makes the glGetUniformLocation calls look nicer lol

		//TODO: don't do this every frame
		//Set light properties
		GLint lightPositionLoc = glGetUniformLocation(sp.shaderProgram, glarg("position"));
		GLint lightDiffuseLoc = glGetUniformLocation(sp.shaderProgram, glarg("diffuse"));
		GLint lightSpecularLoc = glGetUniformLocation(sp.shaderProgram, glarg("specular"));

		#undef glarg

		glUniform3f(lightPositionLoc, dirLights[i]->direction.x, dirLights[i]->direction.y, dirLights[i]->direction.z);
		glUniform3f(lightDiffuseLoc, dirLights[i]->diffuse.r, dirLights[i]->diffuse.g, dirLights[i]->diffuse.b);
		glUniform3f(lightSpecularLoc, dirLights[i]->specular.r, dirLights[i]->specular.g, dirLights[i]->specular.b);
	}

	GLint ambientLoc = glGetUniformLocation(sp.shaderProgram, "uniAmbient");
    
    //TODO: Don't hardcode ambient value
    glUniform3f(ambientLoc, 0.1f, 0.1f, 0.1f);
	
	go.Draw(camera);
}


//TODO: Update projection matrix to allow different aspect ratios
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0,0, width, height);
    static_cast<global_values*>(glfwGetWindowUserPointer(window))->WIDTH = width;
    static_cast<global_values*>(glfwGetWindowUserPointer(window))->HEIGHT = height;
    static_cast<global_values*>(glfwGetWindowUserPointer(window))->hasResized = true;
}

