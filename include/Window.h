#pragma once

#include <stdexcept>

#include "ShaderManager.h"

#include <GLFW/glfw3.h>

class WindowError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class Window {
  public:
    static inline gl_version_t gl_version;
    static inline int width = 0;
    static inline int height = 0;
    static inline bool hasResized = false;

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

    // TODO: Update projection matrix to allow different aspect ratios
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    
    void ReleaseMouse();
    void CaptureMouse();

  private:
    Window() = default;
    ~Window();

    GLFWwindow* window;
};
