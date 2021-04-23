#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <cstdio>

#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <tuple>
#include <vector>
#include <limits>

#include "Camera.h"
#include "ConfigManager.h"
#include "CubeObject.h"
#include "GameObject.h"
#include "InputManager.h"
#include "PostProcess.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "TimeManager.h"
#include "UIManager.h"
#include "Window.h"
// TODO: clean up duplicate includes

#include "DirLight.h" //TODO:  shouldn't have to include both types of light
#include "PointLight.h"
#include "SpotLight.h"
#include "Flashlight.h"

#include "MeshBase.h"
#include "ProceduralMesh.h"

#include "version.h"

// TODO: figure out where to put these, avoid extern in other files
double lastX, lastY;
double yaw, pitch;

template <typename T> using vec_uniq = std::vector<std::unique_ptr<T>>;

void render(const GameObject& go, const vec_uniq<Light>& lights, const Camera& camera);

int main() {
    std::filesystem::current_path("..");

    ConfigManager cm{};

    Window& window = Window::GetInstance();

    window.width = cm.width;
    window.height = cm.height;

    window.gl_version = cm.gl_version;

    try {
        window.Create();
    } catch (const WindowError& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    InputManager& im = InputManager::GetInstance();

    Camera camera{
        {2.0f, 2.0f, 2.0f}, {0.0f, 0.0f, 0.0f}, 2.5f
    };

    int load_result =
        (window.gl_version.is_gles ? gladLoadGLES2Loader : gladLoadGLLoader)((GLADloadproc)glfwGetProcAddress);

    if (load_result == 0) {
        std::cerr << "Error initializing glad" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glEnable(GL_MULTISAMPLE);

    // A fullscreen quad
    // The scene is rendered to a texture and the texture is applied to the quad
    PostProcess& fsq = PostProcess::GetInstance();

    using Direction = Camera::Direction;

    // Create a lambda that translates the camera in a certain direction
    auto translateCamera = [&camera, &deltaTime = timeManager.deltaTime](Direction d) {
        return [&, d] {
            // TODO: Get key bindings from files
            // TODO: Figure out how to use control key

            const auto& [front, back, right, left, up, down] = camera.vectors;

            glm::vec3 vectors[]{front, back, right, left, up, down};

            glm::vec3 vector = vectors[static_cast<int>(d)];

            glm::mat4 translation = glm::translate(glm::mat4{1.0f}, camera.speed * static_cast<float>(deltaTime) * vector);
            camera.Translate(translation);
        };
    };

    using KeyState = InputManager::KeyState;

#define KEY(k) GLFW_KEY_##k

    // Set key bindings
    // TODO: Set in config file
    im.AddKeyBinding(KEY(W), KeyState::AnyPress, translateCamera(Direction::Forward));
    im.AddKeyBinding(KEY(A), KeyState::AnyPress, translateCamera(Direction::Left));
    im.AddKeyBinding(KEY(S), KeyState::AnyPress, translateCamera(Direction::Backward));
    im.AddKeyBinding(KEY(D), KeyState::AnyPress, translateCamera(Direction::Right));
    im.AddKeyBinding(KEY(SPACE), KeyState::AnyPress, translateCamera(Direction::Up));
    im.AddKeyBinding(KEY(C), KeyState::AnyPress, translateCamera(Direction::Down));
    im.AddKeyBinding(KEY(ESCAPE), KeyState::InitialPress, [&] { window.Close(); });
    im.AddKeyBinding(KEY(R), KeyState::InitialPress, [&] {
        static bool toggled = false;
        if (toggled) {
            fsq.ReloadShader("shaders/vert_postprocess.glsl", "shaders/frag_postprocess_passthrough.glsl",
                             window.gl_version);
            toggled = false;
        } else {
            fsq.ReloadShader("shaders/vert_postprocess.glsl", "shaders/frag_postprocess_sobel.glsl",
                             window.gl_version);
            toggled = true;
        }
    });

    im.AddKeyBinding(KEY(U), KeyState::InitialPress, [&window, &im] {
        if (window.showCursor) {
            window.CaptureMouse();
            im.EnableInput();

            UIManager::GetInstance().guiActive = false;

            window.showCursor = false;
        } else {
            InputManager::firstMouse = true; // Prevent camera from jumping to previous cursor location

            window.ReleaseMouse();
            im.DisableInput();

            UIManager::GetInstance().guiActive = true;

            window.showCursor = true;
        }
    });

    // TODO: Add AssetManager, like TextureManager but for all assets
    // Compile and link shaders
    ShaderProgram& cubeShader =
        shaderManager.AddShader({"shaders/vert_cube.glsl", "shaders/frag_cube.glsl", window.gl_version});
    ShaderProgram& lightShader =
        shaderManager.AddShader({"shaders/vert_light.glsl", "shaders/frag_light.glsl", window.gl_version});

    // TODO: use different format
    Mesh floorMesh{"data/floor.fbx"};
    Mesh mesh{"data/cube_irreg.fbx"};
    Mesh lightMesh{"data/cube.fbx"};

    SphereMesh procMesh{};

    // Create textures
    TextureManager texman;

    try {
        texman.AddTextureFromFile("container", "container2.png");
        texman.AddTextureFromFile("container_specular", "container2_specular.png");
        texman.AddTextureFromFile("normalmaptest1", "normalmaptest1.png");
        texman.AddTextureFromFile("puppy", "sample2.png");
        texman.AddTextureFromFile("gradient", "gradient.png");
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

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
    auto go = std::make_unique<CubeObject>(mesh, cubeShader, glm::mat4{1.0f}, texman);
    go->name = "cube1";
    go->SetupTextures();

    auto go2 =
        std::make_unique<CubeObject>(procMesh, cubeShader, glm::translate(glm::mat4{1.0f}, glm::vec3{0, 0, 2.0f}), texman);
    go2->name = "sphere1";
    go2->texture_name = "gradient";

    glm::mat4 floorTransform = glm::translate(glm::mat4{1.0f}, {0.0f, -1.5f, 0.0f});
    floorTransform = glm::rotate(floorTransform, glm::radians(90.0f), {1.0f, 0.0f, 0.0f});
    floorTransform = glm::scale(floorTransform, {5.0f, 5.0f, 1.0f});
    auto floor =
        std::make_unique<CubeObject>(floorMesh, cubeShader, floorTransform, texman);
    floor->texture_name = "container";
    floor->spec_texture_name = "container_specular";

    vec_uniq<Light> lights;

    auto pointLight = std::make_unique<PointLight>(glm::vec3{3.0f, 1.0f, 2.0f}, glm::vec3{0.5f}, glm::vec3{1.0f});
    auto pointLight2 = std::make_unique<PointLight>(glm::vec3{-6.0f, 1.0f, -2.0f}, glm::vec3{0.5f}, glm::vec3{1.0f});
    lights.push_back(std::move(pointLight));
    lights.push_back(std::move(pointLight2));

    auto dirLight = std::make_unique<DirLight>(glm::vec3{-1.0f, 0.0f, 0.0f}, glm::vec3{3.0f}, glm::vec3{3.0f});

    lights.push_back(std::move(dirLight));

    auto spotLight = std::make_unique<SpotLight>(glm::vec3{3.0f, 0.75f, 0.0f}, glm::vec3{-1.0f, -0.25f, 0.0f},
                                                  glm::vec3{3.0f}, glm::vec3{3.0f}, glm::cos(glm::radians(15.5f)));
    lights.push_back(std::move(spotLight));

    auto flashlight = std::make_unique<Flashlight>(glm::vec3{-1.0f, -0.25f, 0.0f},
                                                 glm::vec3{3.0f}, glm::vec3{3.0f}, glm::cos(glm::radians(15.5f)), camera);
    lights.push_back(std::move(flashlight));

    std::size_t flashlight_idx = lights.size() - 1;

    // TODO: Create LightObject class
    // The white cubes that represent lights
    vec_uniq<GameObject> lightObjects;

    // TODO: Light objects appear in the wrong place compared to the location of the light
    for (size_t i = 0; i < lights.size(); i++) {
        if (lights[i]->type == Light::LightType::Point) {
            auto light = static_cast<PointLight*>(lights[i].get());
            glm::mat4 lightTransform{1.0f};
            lightTransform =
                glm::translate(glm::scale(lightTransform, glm::vec3{0.5f}),
                               glm::vec3{light->position.x, light->position.y,
                                         light->position.z}); // Scale by 0.5 then translate to correct position
            auto lo = std::make_unique<CubeObject>(lightMesh, lightShader, lightTransform, texman);
            lightObjects.push_back(std::move(lo));
        }
    }

    im.AddKeyBinding(KEY(F), KeyState::InitialPress,
                     [&lights, flashlight_idx] { lights[flashlight_idx]->ToggleActive(); });

    // main loop
    while (!window.ShouldClose()) {
        // Enable depth test when rendering main scene
        glEnable(GL_DEPTH_TEST);

        // Render main scene to the framebuffer's texture
        fsq.BindFramebuffer();

        if (window.hasResized) {
            glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)window.width / window.height, 1.0f, 10.0f);

            for (const auto& [id, sp] : shaderManager.GetMap()) {
                glUseProgram(sp.shaderProgram);
                GLint uniProj = glGetUniformLocation(sp.shaderProgram, "proj");
                glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
            }

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

        render(*go, lights, camera);
        render(*go2, lights, camera);
        render(*floor, lights, camera);

        // Render all of the lights
        for (auto& lo : lightObjects) {
            lo->Tick();
            render(*lo, lights, camera);
        }

        // Unbind the framebuffer and draw the fullscreen quad with the main scene as the texture
        fsq.UnbindFramebuffer();

        // Draw the fullscreen quad
        fsq.Draw();

        UIManager::GetInstance().Draw();

        window.SwapBuffers();

        timeManager.Tick();
    }

    window.Destroy();

    return EXIT_SUCCESS;
}

void render(const GameObject& go, const vec_uniq<Light>& lights, const Camera& camera) {
    const auto& sp = go.shaderProgram;

    glUseProgram(sp.shaderProgram);

    GLint numLightsUniform = glGetUniformLocation(sp.shaderProgram, "numLights");
    
    std::size_t numLights = lights.size();
    
    if (numLights < 0 || numLights > std::numeric_limits<GLint>::max()) {
        std::cerr << "Invalid number of lights" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    glUniform1i(numLightsUniform, static_cast<GLint>(numLights));

    for (std::size_t i = 0; i < lights.size(); i++) {
        // TODO: don't do this every frame
        lights[i]->SetUniforms(sp.shaderProgram, i);
    }

    GLint ambientLoc = glGetUniformLocation(sp.shaderProgram, "uniAmbient");

    float ambient = 0.5f;
    // TODO: Don't hardcode ambient value
    glUniform3f(ambientLoc, ambient, ambient, ambient);

    go.Draw(camera);
}
