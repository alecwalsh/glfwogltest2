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

#include "Camera.hpp"
#include "ConfigManager.hpp"
#include "CubeObject.hpp"
#include "RenderableObject.hpp"
#include "InputManager.hpp"
#include "PostProcess.hpp"
#include "ShaderManager.hpp"
#include "TextureManager.hpp"
#include "TimeManager.hpp"
#include "UIManager.hpp"
#include "Window.hpp"
// TODO: clean up duplicate includes

#include "DirLight.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"
#include "Flashlight.hpp"

#include "Mesh.hpp"
#include "ProceduralMesh.hpp"

#include "version.hpp"

// TODO: figure out where to put these, avoid extern in other files
double lastX, lastY;
double yaw, pitch;

template <typename T> using vec_uniq = std::vector<std::unique_ptr<T>>;

void render(const RenderableObject& go, const vec_uniq<Light>& lights, const Camera& camera);

using glm::vec3, glm::mat4;

int main() {
    std::filesystem::current_path(BASE_DIR);

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
        {3.0f, 3.0f, 3.0f}, {0.0f, 0.0f, 0.0f}, 2.5f
    };

    glEnable(GL_MULTISAMPLE);

    // A fullscreen quad
    // The scene is rendered to a texture and the texture is applied to the quad
    PostProcess& fsq = PostProcess::GetInstance();

    using Direction = Camera::Direction;

    // Create a lambda that translates the camera in a certain direction
    auto translateCamera = [&camera](Direction d) {
        return [&, d] {
            // TODO: Get key bindings from files
            // TODO: Figure out how to use control key

            camera.ModifyPosition(camera.speed * static_cast<float>(timeManager.deltaTime) * camera.vectors[d]);
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

    cubeShader.SetupTextures();

    // TODO: use different format
    PlaneMesh floorMesh{};
    Mesh mesh{"data/cube_irreg.fbx"};
    CuboidMesh lightMesh{};

    SphereMesh sphereMesh{};

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
    const auto& cf = camera.vectors.front;

    pitch = glm::degrees(asin(cf.y));
    yaw = glm::degrees(acos(cf.x / cos(asin(cf.y))));

    if (cf.z < 0) {
        yaw = -yaw;
    }

    // Creates a CubeObject
    auto go = std::make_unique<CubeObject>(mesh, cubeShader, texman);
    go->SetPosition({0.0f, 25.0f, 0.0f});
    go->name = "cube1";
    go->SetCollider<Physics::SimpleCubeCollider>();

    auto go2 = std::make_unique<CubeObject>(sphereMesh, cubeShader, texman);
    go2->SetPosition({0, 0, 2.0f});
    go2->name = "sphere1";
    go2->texture_name = "gradient";

    auto go3 = std::make_unique<CubeObject>(sphereMesh, cubeShader, texman);
    go3->SetPosition({0, 5.0f, 2.0f});
    go3->name = "sphere2";
    go3->texture_name = "gradient";
    go3->SetCollider<Physics::SphereCollider>();
    
    mat4 floorRotation = glm::rotate(mat4{1.0f}, glm::radians(90.0f), {-1.0f, 0.0f, 0.0f});
    auto floor =
        std::make_unique<CubeObject>(floorMesh, cubeShader, texman);
    floor->SetScale({10.0f, 10.0f, 1.0f});
    floor->rotation = floorRotation;
    floor->name = "floor";
    floor->texture_name = "container";
    floor->spec_texture_name = "container_specular";

    vec_uniq<Light> lights;

    auto pointLight = std::make_unique<PointLight>(vec3{3.0f, 1.0f, 2.0f}, vec3{0.5f}, vec3{1.0f});
    auto pointLight2 = std::make_unique<PointLight>(vec3{-6.0f, 1.0f, -2.0f}, vec3{0.5f}, vec3{1.0f});
    lights.push_back(std::move(pointLight));
    lights.push_back(std::move(pointLight2));

    auto dirLight = std::make_unique<DirLight>(vec3{-1.0f, 0.0f, 0.0f}, vec3{3.0f}, vec3{3.0f});

    lights.push_back(std::move(dirLight));

    auto spotLight = std::make_unique<SpotLight>(vec3{3.0f, 0.75f, 0.0f}, vec3{-1.0f, -0.25f, 0.0f},
                                                  vec3{3.0f}, vec3{3.0f}, glm::cos(glm::radians(15.5f)));
    lights.push_back(std::move(spotLight));

    auto flashlight = std::make_unique<Flashlight>(vec3{-1.0f, -0.25f, 0.0f},
                                                 vec3{3.0f}, vec3{3.0f}, glm::cos(glm::radians(15.5f)), camera);
    flashlight->ToggleActive();
    lights.push_back(std::move(flashlight));

    std::size_t flashlight_idx = lights.size() - 1;

    // TODO: Create LightObject class
    // The white cubes that represent lights
    vec_uniq<RenderableObject> lightObjects;

    // TODO: Light objects appear in the wrong place compared to the location of the light
    for (size_t i = 0; i < lights.size(); i++) {
        if (lights[i]->type == Light::LightType::Point) {
            auto light = static_cast<PointLight*>(lights[i].get());

            auto lo = std::make_unique<CubeObject>(lightMesh, lightShader, texman);
            // Scale by 0.5 then translate to correct position
            lo->SetScale(vec3{0.5f});
            lo->SetPosition({light->position.x, light->position.y, light->position.z});

            lightObjects.push_back(std::move(lo));
        }
    }

    im.AddKeyBinding(KEY(F), KeyState::InitialPress,
                     [&lights, flashlight_idx] { lights[flashlight_idx]->ToggleActive(); });

    TimeManagerShim tms{timeManager.elapsedTime, timeManager.deltaTime};
    Physics::timeManager = &tms;

    uiManager.Initialize();
    timeManager.Initialize();

    // main loop
    while (!window.ShouldClose()) {
        // Enable depth test when rendering main scene
        glEnable(GL_DEPTH_TEST);

        // Render main scene to the framebuffer's texture
        fsq.BindFramebuffer();

        if (window.hasResized) {
            std::cout << "Window resized" << std::endl;

            shaderManager.UpdateProjectionMatrix(window.width, window.height);

            fsq.Resize();

            window.hasResized = false;
        }

        im.HandleInput();

        if (InputManager::mouseMoved) {
            InputManager::mouseMoved = false;
            camera.Rotate(pitch, yaw);
        }

        camera.Tick();

        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        go->Tick();
        go3->Tick();

        render(*go, lights, camera);
        render(*go2, lights, camera);
        render(*go3, lights, camera);
        render(*floor, lights, camera);

        // Render all of the lights
        for (auto& lo : lightObjects) {
            //lo->Tick();
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

void render(const RenderableObject& go, const vec_uniq<Light>& lights, const Camera& camera) {
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
