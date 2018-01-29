#pragma once

#include <GLFW/glfw3.h>

#include <tuple>

//Major version, minor version, OpenGL or OpenGL ES
using gl_version_t = std::tuple<int, int, bool>;

class Window {
public:
    static gl_version_t gl_version;
    static int width;
    static int height;
    static bool hasResized;
    
    //Need to set gl_version, width, and height before calling this
    static Window& GetInstance();
    //Deleted to prevent copies
    Window(const Window&) = delete;
    void operator=(const Window&) = delete;
    
    void SwapBuffers();
    bool ShouldClose();
    //Calls glfwSetWindowShouldClose, will close the window on next frame
    void Close();
    
    void Resize(int width, int height);
    
    // TODO: Update projection matrix to allow different aspect ratios
    static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    
private:
    Window(int width, int height, gl_version_t gl_version);
    ~Window();
    
    GLFWwindow* window;
};
