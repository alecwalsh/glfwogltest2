#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <cstdio>
#ifdef __unix__
#include <unistd.h>
#endif

#include <iostream>
#include <sstream>
#include <chrono>
#include <functional>
#include <memory>
#include <vector>

#include "Camera.h"
#include "CubeObject.h"
#include "DirLight.h" //TODO:  shouldn't have to include both types of light
#include "PointLight.h"
#include "SpotLight.h"
#include "GameObject.h"
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

using gl_version_t = std::tuple<int, int, bool>;

gl_version_t gl_version;

//TODO: Move to separate file
struct FullscreenQuad {
    const GLfloat vertices[20] = {
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };
    const GLubyte elements[6] = {0, 1, 2, 0, 2, 3};
    ShaderProgram shader;
    GLuint vao;
    struct buffers {
        GLuint vbo;
        GLuint ebo;
    } buffers;
    GLuint fbo;
    GLuint rbo; //Renderbuffer for depth attachment
    GLuint fb_texture; //Texture for color attachment
    void BindFramebuffer() {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }
    void UnbindFramebuffer() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void SetupFramebuffer() {
        glGenFramebuffers(1, &fbo);
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Framebuffer bound" << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        
        glGenTextures(1, &fb_texture);
        glBindTexture(GL_TEXTURE_2D, fb_texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb_texture, 0);
        
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 800, 600);  
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
        
        glUseProgram(shader.shaderProgram);
        glUniform1i(glGetUniformLocation(shader.shaderProgram, "texFramebuffer"), 0);
    }
    void Draw() {
        glUseProgram(shader.shaderProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fb_texture);
        
        glBindVertexArray(vao);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
    }
    //gl_version needs to be set before insantiating this struct with the default constructor
    FullscreenQuad() : FullscreenQuad{gl_version} {}
    FullscreenQuad(gl_version_t version) : shader{"shaders/vert_postprocess.glsl", "shaders/frag_postprocess.glsl", version} {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        glGenBuffers(2, (GLuint*)&buffers);
        
        glBindBuffer(GL_ARRAY_BUFFER, buffers.vbo);
        glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(vertices[0]), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(elements[0]), elements, GL_STATIC_DRAW);
        
        GLint posAttrib = 0;
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        
        GLint texAttrib = 1;
        glEnableVertexAttribArray(texAttrib);
        glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(0);
        
        SetupFramebuffer();
    }
    ~FullscreenQuad() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(2, (GLuint*)&buffers);
        glDeleteFramebuffers(1, &fbo);
    }
};

// Prototypes for input handling callbacks
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void handle_movement(global_values *gv, Camera &camera, float deltaTime);

template <typename T> using vec_uniq = std::vector<std::unique_ptr<T>>;

void render(const GameObject &go, const vec_uniq<PointLight> &pointLights, const vec_uniq<DirLight> &dirLights, const vec_uniq<SpotLight> &spotLights,
            const Camera &camera);

// TODO: avoid globals, use glfwSetWindowUserPointer
bool keys[1024];

int main(int argc, char *argv[]) {
#ifdef __unix__
    chdir(".."); // Data files and shaders are in parent directory
#elif !defined(_WIN32)
    //TODO: Find out how to change directory on Windows
    #error "Not implemented yet: chdir"
#endif
    auto t_start = std::chrono::high_resolution_clock::now();
    auto t_prev = t_start;
    float elapsedTime = 0.0f;
    float deltaTime = 0.0f;

    glfwInit();
    
    ConfigManager cm{};
    
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
    
    global_values gs{};
    
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

    glfwSetWindowUserPointer(window, static_cast<void*>(&gs));

    int load_result = gl_es ? gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)
                            : gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (!load_result) {
        std::cerr << "Error initializing glad" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glEnable(GL_MULTISAMPLE);
    
    gl_version = std::tie(gl_major_version, gl_minor_version, gl_es);
    
    //A fullscreen quad
    //The scene is rendered to a texture and the texture is applied to the quad
    FullscreenQuad fsq{};

    //TODO: Add AssetManager, like TextureManager but for all assets
    // Compile and link shaders
    ShaderProgram cubeShader{"shaders/vert_cube.glsl", "shaders/frag_cube.glsl", gl_version};
    ShaderProgram lightShader{"shaders/vert_light.glsl", "shaders/frag_light.glsl", gl_version};

    // TODO: use .blend files
    Mesh floorMesh{"data/floor.fbx"};
    Mesh mesh{"data/cube_irreg.fbx"};
    Mesh lightMesh{"data/cube.fbx"};
    

    // Create textures
    TextureManager texman;
    texman.AddTextureFromFile("container", "container2.png");
    texman.AddTextureFromFile("container_specular", "container2_specular.png");
    texman.AddTextureFromFile("normalmaptest1", "normalmaptest1.png");
    texman.AddTextureFromFile("puppy", "sample2.png");

    Camera camera{
        {2.0f, 2.0f, 2.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} // y-axis is up
    };

    // Sets pitch and yaw based on the cameraFront vector;  this prevents the camera from jumping when moving the mouse
    // for the first time
    // This is just the inverse of the code in Camera::Rotate
    auto& cf = camera.cameraFront;

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
    
    ls.Register("Tick", &CubeObject::Tick, go.get());
    ls.Register("LambdaTest", []{std::cout << "Lambda called from Lua\n";});
    ls.Register("elapsedTime", &elapsedTime, LuaScript::Type::Float);
    ls.Register("RotSpeed", &go->RotSpeed, LuaScript::Type::Float);

    glm::mat4 floorTransform = glm::translate(glm::mat4(), {0.0f, -1.5f, 0.0f});
    floorTransform = glm::rotate(floorTransform, glm::radians(90.0f), {1.0f, 0.0f, 0.0f});
    floorTransform = glm::scale(floorTransform, {5.0f, 5.0f, 1.0f});
    auto floor = std::make_unique<CubeObject>(floorMesh, cubeShader, floorTransform, elapsedTime, deltaTime, texman);
    floor->texture_name = "container";
    floor->spec_texture_name = "container_specular";

    vec_uniq<PointLight> pointLights;
    auto pointLight = std::make_unique<PointLight>(glm::vec3(3.0f, 1.0f, 2.0f), glm::vec3(0.5f), glm::vec3(1.0f));
    auto pointLight2 = std::make_unique<PointLight>(glm::vec3(-6.0f, 1.0f, -2.0f), glm::vec3(0.5f), glm::vec3(1.0f));
    pointLights.push_back(std::move(pointLight));
    pointLights.push_back(std::move(pointLight2));

    vec_uniq<DirLight> dirLights;
    auto dirLight = std::make_unique<DirLight>(glm::vec3(0.0f, -0.75f, 1.0f), glm::vec3(1.5f), glm::vec3(0.5f));
    dirLights.push_back(std::move(dirLight));
    
    vec_uniq<SpotLight> spotLights;
    auto spotLight = std::make_unique<SpotLight>(glm::vec3(3.0f, 0.75f, 0.0f), glm::vec3(-1.0f, -0.25f, 0.0f), glm::vec3(3.0f), glm::vec3(3.0f), glm::cos(glm::radians(15.5f)));
    spotLights.push_back(std::move(spotLight));

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
        
        //Enable depth test when rendering main scene
        glEnable(GL_DEPTH_TEST);
        
        //Render main scene to the framebuffer's texture
        fsq.BindFramebuffer();
        
        if (gs.hasResized) {
            //TODO: do this for all shaders automatically, instead of manually
            glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)gs.WIDTH / gs.HEIGHT, 1.0f, 10.0f);
            
            glUseProgram(cubeShader.shaderProgram);
            GLint uniProj = glGetUniformLocation(cubeShader.shaderProgram, "proj");
            glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

            glUseProgram(lightShader.shaderProgram);
            GLint uniProj2 = glGetUniformLocation(lightShader.shaderProgram, "proj");
            glUniformMatrix4fv(uniProj2, 1, GL_FALSE, glm::value_ptr(proj));
        }

        glfwPollEvents();
        handle_movement(&gs, camera, deltaTime);

        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ls.exec("loop()");
//         lua_pcall(ls.L, 0, 0, 0);
        
        render(*go, pointLights, dirLights, spotLights, camera);
        render(*floor, pointLights, dirLights, spotLights, camera);

        for (size_t i = 0; i < lightObjects.size(); i++) {
            lightObjects[i]->Tick();
            render(*lightObjects[i], pointLights, dirLights, spotLights, camera);
        }
        
        //Unbind the framebuffer and draw the fullscreen quad with the main scene as the texture
        fsq.UnbindFramebuffer();
    
        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        //Depth test is unnecessary here because we are rendering a single quad
        glDisable(GL_DEPTH_TEST);
        
        //Draw the fullscreen quad
        fsq.Draw();
        
        // Swap buffers
        glfwSwapBuffers(window);
    }

    // std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "bye" << std::endl;

    glfwTerminate();

    return EXIT_SUCCESS;
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
void render(const GameObject &go, const vec_uniq<PointLight> &pointLights, const vec_uniq<DirLight> &dirLights, const vec_uniq<SpotLight> &spotLights,
            const Camera &camera) {
    const auto &sp = go.shaderProgram;

    glUseProgram(sp.shaderProgram);

    GLint numPointLights = glGetUniformLocation(sp.shaderProgram, "numPointLights");
    glUniform1i(numPointLights, pointLights.size());

    GLint numDirLights = glGetUniformLocation(sp.shaderProgram, "numDirLights");
    glUniform1i(numDirLights, dirLights.size());
    
    GLint numSpotLights = glGetUniformLocation(sp.shaderProgram, "numSpotLights");
    glUniform1i(numSpotLights, spotLights.size());

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
        glUniform3f(getDirLightUniLoc("direction"), dirLights[i]->direction.x, dirLights[i]->direction.y,
                    dirLights[i]->direction.z);
        glUniform3f(getDirLightUniLoc("diffuse"), dirLights[i]->diffuse.r, dirLights[i]->diffuse.g,
                    dirLights[i]->diffuse.b);
        glUniform3f(getDirLightUniLoc("specular"), dirLights[i]->specular.r, dirLights[i]->specular.g,
                    dirLights[i]->specular.b);
    }
    
    for (size_t i = 0; i < spotLights.size(); i++) {
        // Get the uniform location for directional lights
        auto getSpotLightUniLoc = std::bind(getLightUniLoc, _1, i, "spotLights");

        // TODO: don't do this every frame
        // Set light properties
        //TODO: Support regular spotlights and flashlights
        //Use the spotlight's position and direction
//         glUniform3f(getSpotLightUniLoc("position"), spotLights[i]->position.x, spotLights[i]->position.y,
//                     spotLights[i]->position.z);
//         glUniform3f(getSpotLightUniLoc("direction"), spotLights[i]->direction.x, spotLights[i]->direction.y,
//                     spotLights[i]->direction.z);
        //Set the position and direction to the camera's, like a flashlight
        glUniform3f(getSpotLightUniLoc("position"), camera.position.x, camera.position.y,
                    camera.position.z);
        glUniform3f(getSpotLightUniLoc("direction"), camera.cameraFront.x, camera.cameraFront.y,
                    camera.cameraFront.z);
        glUniform3f(getSpotLightUniLoc("diffuse"), spotLights[i]->diffuse.r, spotLights[i]->diffuse.g,
                    spotLights[i]->diffuse.b);
        
        glUniform3f(getSpotLightUniLoc("diffuse"), spotLights[i]->diffuse.r, spotLights[i]->diffuse.g,
                    spotLights[i]->diffuse.b);
        glUniform3f(getSpotLightUniLoc("specular"), spotLights[i]->specular.r, spotLights[i]->specular.g,
                    spotLights[i]->specular.b);
        glUniform1f(getSpotLightUniLoc("cutoffAngle"), spotLights[i]->cutoffAngle);
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
