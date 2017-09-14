#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <SOIL/SOIL.h>

#include <cstdio>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <chrono>
#include <functional>
#include <memory>
#include <vector>

#include "Camera.h"
#include "CubeObject.h"
#include "DirLight.h" //TODO:  shouldn't have to include both types of light
#include "GameObject.h"
#include "PointLight.h"
#include "ShaderProgram.h"
#include "TextureManager.h"
#include "ConfigManager.h"
// TODO: clean up duplicate includes

// TODO: move this to separate file
struct global_values {
    GLuint WIDTH, HEIGHT;
    float lastX, lastY;
    double yaw, pitch;
    bool hasResized = false;
};

// Prototypes for input handling callbacks
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void handle_movement(global_values *gv, Camera &camera, float deltaTime);

template <typename T> using vec_uniq = std::vector<std::unique_ptr<T>>;

void render(const GameObject &go, const vec_uniq<PointLight> &pointLights, const vec_uniq<DirLight> &dirLights,
            const Camera &camera);

// TODO: avoid globals, use glfwSetWindowUserPointer
bool keys[1024];

int main(int argc, char *argv[]) {
    chdir(".."); // Data files and shaders are in parent directory
    auto t_start = std::chrono::high_resolution_clock::now();
    auto t_prev = t_start;
    float elapsedTime = 0.0f;
    float deltaTime = 0.0f;

    glfwInit();
    
    ConfigManager cm;
    
    LuaScript ls{"bind.lua"};

    // TODO: Switch between GL and GLES with command line switch
    auto gl_major_version = 3;
    auto gl_minor_version = 3;
    bool gl_es = false;

    int gl_api = gl_es ? GLFW_OPENGL_ES_API : GLFW_OPENGL_API;

    glfwWindowHint(GLFW_CLIENT_API, gl_api);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_major_version);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_minor_version);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 8);
    
    global_values gs;
    
    gs.WIDTH = cm.width;
    gs.HEIGHT = cm.height;
    
    GLFWwindow *window = glfwCreateWindow(gs.WIDTH, gs.HEIGHT, "OpenGL", nullptr, nullptr); // Windowed
    
    glfwMakeContextCurrent(window);
    
    glfwSwapInterval(1);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set global state and call glfwSetWindowUserPointer to make it accessable to callbacks
    gs.hasResized = false;
    gs.lastX = gs.WIDTH / 2;
    gs.lastY = gs.HEIGHT / 2;

    glfwSetWindowUserPointer(window, &gs);

    int load_result = gl_es ? gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)
                            : gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (!load_result) {
        std::cerr << "Error initializing GLEW";
        glfwTerminate();
        return -1;
    }

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    auto version = std::tie(gl_major_version, gl_minor_version, gl_es);
    // compile and link shaders
    ShaderProgram cubeShader{"shaders/vert_cube.glsl", "shaders/frag_cube.glsl", version};
    ShaderProgram lightShader{"shaders/vert_light.glsl", "shaders/frag_light.glsl", version};

    // TODO: use .blend files
    Mesh floorMesh{"data/floor.fbx"};
    Mesh mesh{"data/cube_irreg.fbx"};
    Mesh lightMesh{"data/cube.fbx"};

    // Create textures
    TextureManager texman;
    texman.AddTexture("container", "data/container2.png");
    texman.AddTexture("container_specular", "data/container2_specular.png");
    texman.AddTexture("normalmaptest1", "data/normalmaptest1.png");
    texman.AddTexture("puppy", "data/sample2.png");

    Camera camera{
        {2.0f, 2.0f, 2.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} // y-axis is up
    };

    // Sets pitch and yaw based on the cameraFront vector;  this prevents the camera from jumping when moving the mouse
    // for the first time
    // This is just the inverse of the code in Camera::Rotate
    auto &cf = camera.cameraFront;

    gs.pitch = glm::degrees(asin(cf.y));
    gs.yaw = glm::degrees(acos(cf.x / cos(asin(cf.y))));

    if (cf.z < 0) {
        gs.yaw = -gs.yaw;
    }

    // Creates a CubeObject
    glm::mat4 transform;
    auto go = std::make_unique<CubeObject>(mesh, cubeShader, transform, elapsedTime, deltaTime, texman);
    go->name = "cube1";
    go->SetupTextures();
    go->LuaRegister(ls);
    
//     auto modTransformLambda = [](int x, int y, int z){};
    
    ls.Register("Tick", &CubeObject::Tick, go.get());
    ls.Register("LambdaTest", []{std::cout << "Lambda called from Lua\n";});
    ls.Register("elapsedTime", &elapsedTime, LuaScript::Type::Float);
    ls.Register("RotSpeed", &go->RotSpeed, LuaScript::Type::Float);

    glm::mat4 floorTransform = glm::translate(glm::mat4(), {0.0f, -1.5f, 0.0f});
    floorTransform = glm::rotate(floorTransform, glm::radians(90.0f), {1.0f, 0.0f, 0.0f});
    floorTransform = glm::scale(floorTransform, {5.0f, 5.0f, 1.0f});
    auto floor = std::make_unique<CubeObject>(floorMesh, cubeShader, floorTransform, elapsedTime, deltaTime, texman);

    vec_uniq<PointLight> pointLights;
    auto pointLight = std::make_unique<PointLight>(glm::vec3(3.0f, 1.0f, 2.0f), glm::vec3(0.5f), glm::vec3(1.0f));
    auto pointLight2 = std::make_unique<PointLight>(glm::vec3(-6.0f, 1.0f, -2.0f), glm::vec3(0.5f), glm::vec3(1.0f));
    pointLights.push_back(std::move(pointLight));
    pointLights.push_back(std::move(pointLight2));

    vec_uniq<DirLight> dirLights;
    auto dirLight = std::make_unique<DirLight>(glm::vec3(0.0f, -0.75f, 1.0f), glm::vec3(0.5f), glm::vec3(0.25f));
    dirLights.push_back(std::move(dirLight));

    // TODO: Create LightObject class
    // The white cubes that represent lights
    vec_uniq<GameObject> lightObjects;

    // TODO: Light objects appear in the wrong place compared to the location of the light
    for (size_t i = 0; i < pointLights.size(); i++) {
        glm::mat4 lightTransform;
        lightTransform =
            glm::translate(glm::scale(lightTransform, glm::vec3(0.5f)),
                           glm::vec3(pointLights[i]->position.x, pointLights[i]->position.y,
                                     pointLights[i]->position.z)); // Scale by 0.5 then translate to correct position
        auto lo = std::make_unique<CubeObject>(lightMesh, lightShader, lightTransform, elapsedTime, deltaTime, texman);
        lightObjects.push_back(std::move(lo));
    }

    // main loop
    while (!glfwWindowShouldClose(window)) {
        auto t_now = std::chrono::high_resolution_clock::now();

        deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_prev).count();
        t_prev = t_now;

        elapsedTime = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();

        if (gs.hasResized) {

            glUseProgram(go->shaderProgram.shaderProgram);
            glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)gs.WIDTH / gs.HEIGHT, 1.0f, 10.0f);
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

        ls.exec("loop()");
//         lua_pcall(ls.L, 0, 0, 0);
        
        render(*go, pointLights, dirLights, camera);
        render(*floor, pointLights, dirLights, camera);

        for (size_t i = 0; i < lightObjects.size(); i++) {
            lightObjects[i]->Tick();
            render(*lightObjects[i], pointLights, dirLights, camera);
        }

        // Swap buffers
        glfwSwapBuffers(window);
    }

    // std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "bye\n";

    glfwTerminate();

    return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (action == GLFW_PRESS) {
        keys[key] = true;
    } else if (action == GLFW_RELEASE) {
        keys[key] = false;
    }

    // When a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if (keys[GLFW_KEY_ESCAPE]) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

bool mouseMoved = false;
bool firstMouse = true;
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    float &lastX = static_cast<global_values *>(glfwGetWindowUserPointer(window))->lastX;
    float &lastY = static_cast<global_values *>(glfwGetWindowUserPointer(window))->lastY;

    mouseMoved = true;
    if (firstMouse) {
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

    auto &yaw = static_cast<global_values *>(glfwGetWindowUserPointer(window))->yaw;
    auto &pitch = static_cast<global_values *>(glfwGetWindowUserPointer(window))->pitch;
    yaw -= deltaX * xSensitivity;
    pitch += deltaY * ySensitivity;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }
}

void handle_movement(global_values *gv, Camera &camera, float deltaTime) // TODO: use arrow keys to move objects
{
    auto &yaw = gv->yaw;
    auto &pitch = gv->pitch;

    glm::mat4 translation;

    // Calculates vectors from the perspective of the camera
    // This allows the camera to work no matter how it is moved and rotated
    glm::vec3 rightVector = glm::normalize(glm::cross(camera.cameraFront, camera.up));
    glm::vec3 leftVector = -rightVector;
    glm::vec3 frontVector = camera.cameraFront;
    glm::vec3 backVector = -frontVector;
    glm::vec3 upVector = camera.up;
    glm::vec3 downVector = -upVector;

    float velocity = 2.5f;

    // TODO: Get key bindings from files
    // TODO: Figure out how to use control key

    // Changes the cameras location in response to keypresses
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) {
        translation = glm::translate(translation, velocity * deltaTime * leftVector);
    }
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
        translation = glm::translate(translation, velocity * deltaTime * rightVector);
    }

    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) {
        translation = glm::translate(translation, velocity * deltaTime * frontVector);
    }
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) {
        translation = glm::translate(translation, velocity * deltaTime * backVector);
    }

    if (keys[GLFW_KEY_SPACE]) {
        translation = glm::translate(translation, velocity * deltaTime * upVector);
    }
    if (keys[GLFW_KEY_C]) {
        translation = glm::translate(translation, velocity * deltaTime * downVector);
    }
    if (keys[GLFW_KEY_Q]) {
        // roll
    }
    if (keys[GLFW_KEY_E]) {
        // roll
    }

    if (mouseMoved) {
        mouseMoved = false;
        camera.Rotate(pitch, yaw);
    }

    camera.Translate(translation);
}

// TODO: Support multiple lights and multiple types of lights
void render(const GameObject &go, const vec_uniq<PointLight> &pointLights, const vec_uniq<DirLight> &dirLights,
            const Camera &camera) {
    const auto &sp = go.shaderProgram;

    glUseProgram(sp.shaderProgram);

    GLint numPointLights = glGetUniformLocation(sp.shaderProgram, "numPointLights");
    glUniform1i(numPointLights, pointLights.size());

    GLint numDirLights = glGetUniformLocation(sp.shaderProgram, "numDirLights");
    glUniform1i(numDirLights, dirLights.size());

    auto getLightUniLoc = [sp = sp.shaderProgram](
        const char *member, int i, const char *lightType) // Gets the uniform location for light struct members
    {
        std::stringstream ss;
        ss << lightType << "[" << i << "]." << member;
        return glGetUniformLocation(sp, ss.str().c_str());
    };

    using namespace std::placeholders;

    // TODO: don't do this every frame
    for (size_t i = 0; i < pointLights.size(); i++) {
        // Get the uniform location for point lights
        auto getPointLightUniLoc = std::bind(getLightUniLoc, _1, i, "pointLights");

        // Set light properties
        glUniform3f(getPointLightUniLoc("position"), pointLights[i]->position.x, pointLights[i]->position.y,
                    pointLights[i]->position.z);
        glUniform3f(getPointLightUniLoc("diffuse"), pointLights[i]->diffuse.r, pointLights[i]->diffuse.g,
                    pointLights[i]->diffuse.b);
        glUniform3f(getPointLightUniLoc("specular"), pointLights[i]->specular.r, pointLights[i]->specular.g,
                    pointLights[i]->specular.b);
    }

    for (size_t i = 0; i < dirLights.size(); i++) {
        // Get the uniform location for directional lights
        auto getDirLightUniLoc = std::bind(getLightUniLoc, _1, i, "dirLights");

        // TODO: don't do this every frame
        // Set light properties
        glUniform3f(getDirLightUniLoc("position"), dirLights[i]->direction.x, dirLights[i]->direction.y,
                    dirLights[i]->direction.z);
        glUniform3f(getDirLightUniLoc("diffuse"), dirLights[i]->diffuse.r, dirLights[i]->diffuse.g,
                    dirLights[i]->diffuse.b);
        glUniform3f(getDirLightUniLoc("specular"), dirLights[i]->specular.r, dirLights[i]->specular.g,
                    dirLights[i]->specular.b);
    }

    GLint ambientLoc = glGetUniformLocation(sp.shaderProgram, "uniAmbient");

    // TODO: Don't hardcode ambient value
    glUniform3f(ambientLoc, 0.1f, 0.1f, 0.1f);

    go.Draw(camera);
}

// TODO: Update projection matrix to allow different aspect ratios
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    static_cast<global_values *>(glfwGetWindowUserPointer(window))->WIDTH = width;
    static_cast<global_values *>(glfwGetWindowUserPointer(window))->HEIGHT = height;
    static_cast<global_values *>(glfwGetWindowUserPointer(window))->hasResized = true;
}
