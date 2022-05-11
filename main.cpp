#include "glad/glad.h"

#include <glm/glm.hpp>

#include "ShaderManager.hpp"
#include "InputManager.hpp"
#include "ConfigManager.hpp"
#include "TimeManager.hpp"
#include "UIManager.hpp"
#include "PostProcess.hpp"
#include "CubeObject.hpp"
#include "Window.hpp"
#include "World.hpp"

#include <filesystem>
#include <memory>
#include <vector>
#include <limits>
#include <iostream>

#include "version.hpp"

template <typename T> using vec_uniq = std::vector<std::unique_ptr<T>>;

void render(const RenderableObject& go, const vec_uniq<Light>& lights, const Camera& camera);

int main() {
    std::filesystem::current_path(BASE_DIR);

    ConfigManager cm{};

    Window& window = Window::GetInstance();

    window.width = cm.width;
    window.height = cm.height;

    window.glVersion = cm.glVersion;

    try {
        window.Create();
    } catch (const WindowError& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    InputManager& im = InputManager::GetInstance();

    glEnable(GL_MULTISAMPLE);

    // A fullscreen quad
    // The scene is rendered to a texture and the texture is applied to the quad
    PostProcess& fsq = PostProcess::GetInstance();

    GameEngine::World world;

    GameEngine::world = &world;

    TimeManagerShim tms{timeManager.elapsedTime, timeManager.deltaTime};
    Physics::timeManager = &tms;

    uiManager.Initialize();
    timeManager.Initialize();

    // main loop
    while (!window.ShouldClose()) {
        window.MakeContextCurrent();

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

        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Call Tick on everything
        world.TickAll();


        // Now render everything

        // Render all of the GameObjects
        for (const auto& go : world.gameObjects) {
            render(*go, world.lights, world.camera);
        }

        // Render all of the lights
        for (auto& lo : world.lightObjects) {
            render(*lo, world.lights, world.camera);
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

    sp.UseProgram();

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
