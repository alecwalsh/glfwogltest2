#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <cstdio>
#ifdef __unix__
#include <unistd.h>
#elif _WIN32
#include <direct.h>
#endif

#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <tuple>
#include <vector>

#include "Camera.h"
#include "ConfigManager.h"
#include "CubeObject.h"
#include "DirLight.h" //TODO:  shouldn't have to include both types of light
#include "GameObject.h"
#include "InputManager.h"
#include "PointLight.h"
#include "PostProcess.h"
#include "ShaderProgram.h"
#include "SpotLight.h"
#include "TextureManager.h"
#include "Window.h"
// TODO: clean up duplicate includes

#include "version.h"

// TODO: figure out where to put these, avoid extern in other files
float lastX, lastY;
double yaw, pitch;

template <typename T> using vec_uniq = std::vector<std::unique_ptr<T>>;

void render(const GameObject& go, const vec_uniq<Light>& lights, const Camera& camera);

int main(int argc, char* argv[]) {
#ifdef __unix__
    chdir(".."); // Data files and shaders are in parent directory
#elif _WIN32
    _chdir("..");
#endif
    auto t_start = std::chrono::high_resolution_clock::now();
    auto t_prev = t_start;
    float elapsedTime = 0.0f;
    float deltaTime = 0.0f;

    ConfigManager cm{};

    Window::width = cm.width;
    Window::height = cm.height;

    Window::gl_version = cm.gl_version;

    Window& window = Window::GetInstance();

    try {
        window.Create();
    } catch (std::exception& e) {
        std::cout << e.what();
        return 1;
    }

    InputManager& im = InputManager::GetInstance();

    LuaScript ls{"bind.lua"};

    Camera camera{
        {2.0f, 2.0f, 2.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} // y-axis is up
    };

    glfwSwapInterval(1);

    Window::hasResized = false;

    int load_result = std::get<2>(Window::gl_version) ? gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)
                                                      : gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (!load_result) {
        std::cerr << "Error initializing glad" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glEnable(GL_MULTISAMPLE);

    // A fullscreen quad
    // The scene is rendered to a texture and the texture is applied to the quad
    PostProcess& fsq = PostProcess::GetInstance();

    using Direction = Camera::Direction;

    // Translates the camera in a certain direction
    auto translateCamera = [&camera, &deltaTime](Direction d) {
        // TODO: Get key bindings from files
        // TODO: Figure out how to use control key
        glm::vec3 vector;

        switch (d) {
        case Direction::Forward:
            vector = camera.vectors.frontVector;
            break;
        case Direction::Backward:
            vector = camera.vectors.backVector;
            break;
        case Direction::Left:
            vector = camera.vectors.leftVector;
            break;
        case Direction::Right:
            vector = camera.vectors.rightVector;
            break;
        case Direction::Up:
            vector = camera.vectors.upVector;
            break;
        case Direction::Down:
            vector = camera.vectors.downVector;
            break;
        }
        // TODO: set this elsewhere
        float velocity = 2.5f;

        glm::mat4 translation;
        translation = glm::translate(translation, velocity * deltaTime * vector);
        camera.Translate(translation);
    };

    using KeyState = InputManager::KeyState;

    // Set key bindings
    // TODO: Set in Lua file
    im.AddKeyBinding(KEY(W), KeyState::AnyPress, [&] { translateCamera(Direction::Forward); });
    im.AddKeyBinding(KEY(A), KeyState::AnyPress, [&] { translateCamera(Direction::Left); });
    im.AddKeyBinding(KEY(S), KeyState::AnyPress, [&] { translateCamera(Direction::Backward); });
    im.AddKeyBinding(KEY(D), KeyState::AnyPress, [&] { translateCamera(Direction::Right); });
    im.AddKeyBinding(KEY(SPACE), KeyState::AnyPress, [&] { translateCamera(Direction::Up); });
    im.AddKeyBinding(KEY(C), KeyState::AnyPress, [&] { translateCamera(Direction::Down); });
    im.AddKeyBinding(KEY(ESCAPE), KeyState::InitialPress, [&] { window.Close(); });
    im.AddKeyBinding(KEY(R), KeyState::InitialPress, [&] {
        static bool toggled = false;
        if (toggled) {
            fsq.ReloadShader("shaders/vert_postprocess.glsl", "shaders/frag_postprocess_passthrough.glsl",
                             Window::gl_version);
            toggled = false;
        } else {
            fsq.ReloadShader("shaders/vert_postprocess.glsl", "shaders/frag_postprocess_sobel.glsl",
                             Window::gl_version);
            toggled = true;
        }
    });

    // TODO: Add AssetManager, like TextureManager but for all assets
    // Compile and link shaders
    ShaderProgram cubeShader{"shaders/vert_cube.glsl", "shaders/frag_cube.glsl", Window::gl_version};
    ShaderProgram lightShader{"shaders/vert_light.glsl", "shaders/frag_light.glsl", Window::gl_version};

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

    // Sets pitch and yaw based on the cameraFront vector;  this prevents the camera from jumping when moving the mouse
    // for the first time
    // This is just the inverse of the code in Camera::Rotate
    auto& cf = camera.vectors.frontVector;

    pitch = glm::degrees(asin(cf.y));
    yaw = glm::degrees(acos(cf.x / cos(asin(cf.y))));

    if (cf.z < 0) {
        yaw = -yaw;
    }

    // Creates a CubeObject
    glm::mat4 transform;
    auto go = std::make_unique<CubeObject>(mesh, cubeShader, transform, elapsedTime, deltaTime, texman);
    go->name = "cube1";
    go->SetupTextures();
    go->LuaRegister(ls);

    ls.Register("Tick", [&go] { go->Tick(); });
    ls.Register("SetRotSpeed", [&go](double a) { go->RotSpeed = a; });
    ls.Register("LambdaTest", [] { std::cout << "Lambda called from Lua\n" << std::endl; });
    ls.Register("elapsedTime", &elapsedTime, LuaScript::Type::Float);
    ls.Register("RotSpeed", &go->RotSpeed, LuaScript::Type::Float);

    glm::mat4 floorTransform = glm::translate(glm::mat4(), {0.0f, -1.5f, 0.0f});
    floorTransform = glm::rotate(floorTransform, glm::radians(90.0f), {1.0f, 0.0f, 0.0f});
    floorTransform = glm::scale(floorTransform, {5.0f, 5.0f, 1.0f});
    auto floor = std::make_unique<CubeObject>(floorMesh, cubeShader, floorTransform, elapsedTime, deltaTime, texman);
    floor->texture_name = "container";
    floor->spec_texture_name = "container_specular";

    vec_uniq<Light> lights;

    auto pointLight = std::make_unique<PointLight>(glm::vec3(3.0f, 1.0f, 2.0f), glm::vec3(0.5f), glm::vec3(1.0f));
    auto pointLight2 = std::make_unique<PointLight>(glm::vec3(-6.0f, 1.0f, -2.0f), glm::vec3(0.5f), glm::vec3(1.0f));
    lights.push_back(std::move(pointLight));
    lights.push_back(std::move(pointLight2));

    auto dirLight = std::make_unique<DirLight>(glm::vec3(0.0f, -0.75f, 1.0f), glm::vec3(1.5f), glm::vec3(0.5f));
    lights.push_back(std::move(dirLight));

    auto spotLight = std::make_unique<SpotLight>(glm::vec3(3.0f, 0.75f, 0.0f), glm::vec3(-1.0f, -0.25f, 0.0f),
                                                 glm::vec3(3.0f), glm::vec3(3.0f), glm::cos(glm::radians(15.5f)));
    lights.push_back(std::move(spotLight));

    // TODO: Create LightObject class
    // The white cubes that represent lights
    vec_uniq<GameObject> lightObjects;

    // TODO: Light objects appear in the wrong place compared to the location of the light
    for (size_t i = 0; i < lights.size(); i++) {
        if (lights[i]->type == Light::LightType::Point) {
            auto light = static_cast<PointLight*>(lights[i].get());
            glm::mat4 lightTransform;
            lightTransform =
                glm::translate(glm::scale(lightTransform, glm::vec3(0.5f)),
                               glm::vec3(light->position.x, light->position.y,
                                         light->position.z)); // Scale by 0.5 then translate to correct position
            auto lo =
                std::make_unique<CubeObject>(lightMesh, lightShader, lightTransform, elapsedTime, deltaTime, texman);
            lightObjects.push_back(std::move(lo));
        }
    }

    im.AddKeyBinding(KEY(F), KeyState::InitialPress, [&] { lights.back()->ToggleActive(); });

    // main loop
    while (!window.ShouldClose()) {
        auto t_now = std::chrono::high_resolution_clock::now();

        deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_prev).count();
        t_prev = t_now;

        elapsedTime = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();

        // Enable depth test when rendering main scene
        glEnable(GL_DEPTH_TEST);

        // Render main scene to the framebuffer's texture
        fsq.BindFramebuffer();

        if (Window::hasResized) {
            // TODO: do this for all shaders automatically, instead of manually
            glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)window.width / window.height, 1.0f, 10.0f);

            glUseProgram(cubeShader.shaderProgram);
            GLint uniProj = glGetUniformLocation(cubeShader.shaderProgram, "proj");
            glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

            glUseProgram(lightShader.shaderProgram);
            GLint uniProj2 = glGetUniformLocation(lightShader.shaderProgram, "proj");
            glUniformMatrix4fv(uniProj2, 1, GL_FALSE, glm::value_ptr(proj));

            fsq.Resize();
        }

        im.HandleInput();

        if (InputManager::mouseMoved) {
            InputManager::mouseMoved = false;
            camera.Rotate(pitch, yaw);
        }

        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ls.exec("loop()");

        render(*go, lights, camera);
        render(*floor, lights, camera);

        for (size_t i = 0; i < lightObjects.size(); i++) {
            lightObjects[i]->Tick();
            render(*lightObjects[i], lights, camera);
        }

        // Unbind the framebuffer and draw the fullscreen quad with the main scene as the texture
        fsq.UnbindFramebuffer();

        // Draw the fullscreen quad
        fsq.Draw();

        window.SwapBuffers();
    }

    // std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "bye" << std::endl;

    return EXIT_SUCCESS;
}

// TODO: Support multiple lights and multiple types of lights
void render(const GameObject& go, const vec_uniq<Light>& lights, const Camera& camera) {
    const auto& sp = go.shaderProgram;

    glUseProgram(sp.shaderProgram);

    GLint numLights = glGetUniformLocation(sp.shaderProgram, "numLights");
    glUniform1i(numLights, lights.size());

    for (size_t i = 0; i < lights.size(); i++) {
        using Type = Light::LightType;

        auto getLightUniLoc =
            [ sp = sp.shaderProgram, i ](const char* member) // Gets the uniform location for light struct members
        {
            std::stringstream ss;
            ss << "lights[" << i << "]." << member;
            return glGetUniformLocation(sp, ss.str().c_str());
        };

        // TODO: Don't even calculate the lighting for disabled lights in shader.  Add disabled flag to Light struct in
        // shader
        // TODO: Don't hardcode SpotLight here, allow disabling all types of lights
        if (!lights[i]->active) {
            auto light = static_cast<SpotLight*>(lights[i].get());
            glUniform1i(getLightUniLoc("type"), static_cast<int>(light->type));

            // Disable light by setting diffuse and specular to 0
            glUniform3f(getLightUniLoc("diffuse"), 0, 0, 0);
            glUniform3f(getLightUniLoc("specular"), 0, 0, 0);
            continue;
        }

        // TODO: don't do this every frame
        switch (lights[i]->type) {
        case Type::Point: {
            auto light = static_cast<PointLight*>(lights[i].get());
            glUniform1i(getLightUniLoc("type"), static_cast<int>(light->type));
            // Set light properties
            glUniform3f(getLightUniLoc("position"), light->position.x, light->position.y, light->position.z);
            glUniform3f(getLightUniLoc("diffuse"), light->diffuse.r, light->diffuse.g, light->diffuse.b);
            glUniform3f(getLightUniLoc("specular"), light->specular.r, light->specular.g, light->specular.b);
        } break;
        case Type::Directional: {
            auto light = static_cast<DirLight*>(lights[i].get());
            glUniform1i(getLightUniLoc("type"), static_cast<int>(light->type));
            // Set light properties
            glUniform3f(getLightUniLoc("direction"), light->direction.x, light->direction.y, light->direction.z);
            glUniform3f(getLightUniLoc("diffuse"), light->diffuse.r, light->diffuse.g, light->diffuse.b);
            glUniform3f(getLightUniLoc("specular"), light->specular.r, light->specular.g, light->specular.b);
        } break;
        case Type::Spot: {
            auto light = static_cast<SpotLight*>(lights[i].get());
            glUniform1i(getLightUniLoc("type"), static_cast<int>(light->type));
            // Set light properties
            // TODO: Support regular spotlights and flashlights
            // Use the spotlight's position and direction
            //                             glUniform3f(getLightUniLoc("position"), light->position.x, light->position.y,
            //                                         light->position.z);
            //                             glUniform3f(getLightUniLoc("direction"), light->direction.x,
            //                             light->direction.y,
            //                                         light->direction.z);
            // Set the position and direction to the camera's, like a flashlight
            glUniform3f(getLightUniLoc("position"), camera.position.x, camera.position.y, camera.position.z);
            glUniform3f(getLightUniLoc("direction"), camera.vectors.frontVector.x, camera.vectors.frontVector.y,
                        camera.vectors.frontVector.z);

            glUniform3f(getLightUniLoc("diffuse"), light->diffuse.r, light->diffuse.g, light->diffuse.b);
            glUniform3f(getLightUniLoc("specular"), light->specular.r, light->specular.g, light->specular.b);
            glUniform1f(getLightUniLoc("cutoffAngle"), light->cutoffAngle);
        } break;
        default: {
            std::cerr << "Invalid light type" << std::endl;
            exit(EXIT_FAILURE);
        } break;
        }
    }

    GLint ambientLoc = glGetUniformLocation(sp.shaderProgram, "uniAmbient");

    // TODO: Don't hardcode ambient value
    glUniform3f(ambientLoc, 0.1f, 0.1f, 0.1f);

    go.Draw(camera);
}
