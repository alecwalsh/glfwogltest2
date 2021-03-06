#pragma once

#include <stdexcept>

#include "ShaderManager.h"

#include <GLFW/glfw3.h>

class WindowError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class Window {
  public:
    gl_version_t gl_version;
    
    int width = 0;
    int height = 0;

    bool hasResized = false;
    bool showCursor = false;

    // Need to set gl_version, width, and height before calling this
    [[nodiscard]] static Window& GetInstance();
    // Deleted to prevent copies
    Window(const Window&) = delete;
    void operator=(const Window&) = delete;

    void SwapBuffers();
    [[nodiscard]] bool ShouldClose();
    // Calls glfwSetWindowShouldClose, will close the window on next frame
    void Close();

    // Creates the window
    void Create();
    void Destroy();

    void Resize(int width, int height);

    static void framebuffer_size_callback(GLFWwindow* glfwwindow, int width, int height);
    
    void ReleaseMouse();
    void CaptureMouse();
    
    void InitGui();
  private:
    Window() = default;
    ~Window();

    GLFWwindow* window;
};
