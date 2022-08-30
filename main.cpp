#include "glad/glad.h"

#include "InputManager.hpp"
#include "ConfigManager.hpp"
#include "TimeManager.hpp"
#include "UIManager.hpp"
#include "PostProcess.hpp"
#include "CubeObject.hpp"
#include "Window.hpp"
#include "World.hpp"

#include <filesystem>
#include <limits>
#include <iostream>

#include "version.hpp"

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

    // Main loop
    while (!window.ShouldClose()) {
        window.MakeContextCurrent();

        // Enable depth test when rendering main scene
        glEnable(GL_DEPTH_TEST);

        // Render main scene to the framebuffer's texture
        fsq.BindFramebuffer();

        if (window.hasResized) {
            std::cout << "Window resized" << std::endl;

            world.camera.SetWidth(window.width);
            world.camera.SetHeight(window.height);

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
        world.RenderWorld();

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