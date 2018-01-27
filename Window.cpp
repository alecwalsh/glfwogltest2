#include "Window.h"

#include "InputManager.h"

#include <cassert>

extern float lastX, lastY;

int Window::width = 0, Window::height = 0;
bool Window::hasResized = false;

gl_version_t Window::gl_version;

// TODO: Update projection matrix to allow different aspect ratios
void Window::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    
    Window::width = width;
    Window::height = height;
    Window::hasResized = true;
}

Window::Window(int width, int height, gl_version_t gl_version) {
    using std::get;
    
    glfwInit();
    
    auto gl_major_version = get<0>(gl_version);
    auto gl_minor_version = get<1>(gl_version);
    
    int gl_api = get<2>(gl_version) ? GLFW_OPENGL_ES_API : GLFW_OPENGL_API;
    
    glfwWindowHint(GLFW_CLIENT_API, gl_api);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_major_version);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_minor_version);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 8);
    
    window = glfwCreateWindow(width, height, "OpenGL", nullptr, nullptr); // Windowed
    
    glfwMakeContextCurrent(window);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glfwSetKeyCallback(window, InputManager::key_callback);
    glfwSetCursorPosCallback(window, InputManager::mouse_callback);
    glfwSetFramebufferSizeCallback(window, Window::framebuffer_size_callback);
    
    lastX = width/2;
    lastY = height/2;
}

Window::~Window() {
    glfwTerminate();
}

Window& Window::GetInstance() {
    assert(height != 0 && width != 0);
    //TODO: check if gl_version is valid
    static Window w{Window::width, Window::height, Window::gl_version};
    return w;
}

void Window::SwapBuffers() {
    glfwSwapBuffers(window);
}

bool Window::ShouldClose() {
    return glfwWindowShouldClose(window);
}

void Window::Resize(int width, int height) {
    glfwSetWindowSize(window, width, height);
}
