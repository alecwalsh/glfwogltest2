#pragma once

#include "ShaderManager.h"

#include <GLFW/glfw3.h>

class Window {
  public:
    static gl_version_t gl_version;
    static int width;
    static int height;
    static bool hasResized;

    // Need to set gl_version, width, and height before calling this
    static Window& GetInstance();
    // Deleted to prevent copies
    Window(const Window&) = delete;
    void operator=(const Window&) = delete;

    void SwapBuffers();
    bool ShouldClose();
    // Calls glfwSetWindowShouldClose, will close the window on next frame
    void Close();

    // Creates the window
    void Create();
    void Destroy();

    void Resize(int width, int height);

    // TODO: Update projection matrix to allow different aspect ratios
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    
    void ReleaseMouse();
    void CaptureMouse();

  private:
    Window() = default;
    ~Window();

    GLFWwindow* window;
};
