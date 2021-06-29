#pragma once

#include "GLVersion.hpp"

#include <stdexcept>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string_view>

class WindowError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class Window {
  public:
    double lastX;
    double lastY;

    GameEngine::GLVersion glVersion;
    
    int width = 0;
    int height = 0;

    bool hasResized = false;
    bool showCursor = false;

    // Need to set gl_version, width, and height before calling this
    [[nodiscard]] static Window& GetInstance() noexcept;
    // Deleted to prevent copies
    Window(const Window&) = delete;
    void operator=(const Window&) = delete;

    void SwapBuffers() noexcept;
    [[nodiscard]] bool ShouldClose() noexcept;
    // Calls glfwSetWindowShouldClose, will close the window on next frame
    void Close() noexcept;

    // Creates the window
    void Create();
    void Destroy() noexcept;

    void LoadGL();

    // Use char8_t if available
    using GLCharType = decltype(u8' ');
    bool SupportsGLExtension(std::basic_string_view<GLCharType> str);

    void Resize(int width, int height) noexcept;

    static void FramebufferSizeCallback(GLFWwindow* glfwwindow, int width, int height) noexcept;
    
    void ReleaseMouse() noexcept;
    void CaptureMouse() noexcept;
    
    void InitGui() noexcept;
  private:
    Window() = default;
    ~Window();

    GLFWwindow* window;
};
