#include "Window.h"

#include "InputManager.h"
#include "UIManager.h"

#include <cassert>
#include <exception>
#include <iostream>

extern float lastX, lastY;

int Window::width = 0, Window::height = 0;
bool Window::hasResized = false;

gl_version_t Window::gl_version;

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    Window::width = width;
    Window::height = height;
    Window::hasResized = true;
}

void Window::Create() {
    using std::get;

    glfwSetErrorCallback([](int i, const char* desc) {
        std::cout << "GLFW Error" << std::endl;
        std::cout << "  Error code: 0x" << std::hex << i << std::dec << std::endl;
        std::cout << "  Error description: " << desc << std::endl;
    });

    glfwInit();

	auto [gl_major_version, gl_minor_version, uses_gles] = gl_version;

    int gl_api = uses_gles ? GLFW_OPENGL_ES_API : GLFW_OPENGL_API;

    glfwWindowHint(GLFW_CLIENT_API, gl_api);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_major_version);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_minor_version);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 8);

    window = glfwCreateWindow(width, height, "OpenGL", nullptr, nullptr); // Windowed

    if (!window) {
        throw std::runtime_error("Error creating window");
    }

    glfwMakeContextCurrent(window);

    CaptureMouse();

    glfwSetKeyCallback(window, InputManager::key_callback);
    glfwSetCursorPosCallback(window, InputManager::mouse_callback);
    glfwSetFramebufferSizeCallback(window, Window::framebuffer_size_callback);

    lastX = width / 2;
    lastY = height / 2;
}

void Window::InitGui() {
    UIManager::window = window;
    const auto& uim = UIManager::GetInstance();
}

Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

Window& Window::GetInstance() {
    assert(height != 0 && width != 0);
    // TODO: check if gl_version is valid
    static Window w{};
    return w;
}

void Window::SwapBuffers() { glfwSwapBuffers(window); }

bool Window::ShouldClose() { return glfwWindowShouldClose(window); }

void Window::Close() { glfwSetWindowShouldClose(window, true); }

void Window::Resize(int width, int height) { glfwSetWindowSize(window, width, height); }

void Window::ReleaseMouse() { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }

void Window::CaptureMouse() { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }
