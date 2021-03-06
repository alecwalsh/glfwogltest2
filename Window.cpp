#include "Window.h"

#include "InputManager.h"

#include "imgui_impl_glfw.h"

#include <cassert>
#include <iostream>

extern double lastX, lastY;

void Window::framebuffer_size_callback([[maybe_unused]] GLFWwindow* glfwwindow, int width, int height) {
    glViewport(0, 0, width, height);

    auto& window = Window::GetInstance();

    window.width = width;
    window.height = height;
    window.hasResized = true;
}

void Window::Create() {
    // TODO: check if gl_version is valid
    using std::get;

    glfwSetErrorCallback([](int i, const char* desc) {
        auto f = std::cout.flags();

        std::cout << "GLFW Error" << std::endl;
        std::cout << "  Error code: " << std::showbase << std::hex << i << std::endl;
        std::cout << "  Error description: " << desc << std::endl;

        std::cout.flags(f); // Restore flags after using std::hex
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
        throw WindowError{"Error creating window"};
    }

    glfwMakeContextCurrent(window);

    CaptureMouse();

    glfwSetKeyCallback(window, InputManager::key_callback);
    glfwSetCursorPosCallback(window, InputManager::mouse_callback);
    glfwSetFramebufferSizeCallback(window, Window::framebuffer_size_callback);

    lastX = width / 2.0f;
    lastY = height / 2.0f;

    glfwSwapInterval(1);
}

void Window::InitGui() {
    ImGui_ImplGlfw_InitForOpenGL(window, true);
}

void Window::Destroy() { glfwDestroyWindow(window); }

Window::~Window() {
    glfwTerminate();
}

Window& Window::GetInstance() {
    static Window w{};
    return w;
}

void Window::SwapBuffers() { glfwSwapBuffers(window); }

bool Window::ShouldClose() { return glfwWindowShouldClose(window); }

void Window::Close() { glfwSetWindowShouldClose(window, true); }

void Window::Resize(int width, int height) { glfwSetWindowSize(window, width, height); }

void Window::ReleaseMouse() { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }

void Window::CaptureMouse() { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }
