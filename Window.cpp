#include "Window.hpp"

#include "glad/glad.h"

#include "InputManager.hpp"

#include <imgui/imgui_impl_glfw.h>

#include <unordered_set>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

static std::string GLVersionString;

static void SetGLVersionString(GameEngine::GLVersion version) {
    auto suffix = version.is_gles ? "es" : "core";
    GLVersionString = fmt::format("#version {}{}0 {}\n", version.major, version.minor, suffix);
}

static std::unordered_set<std::string_view> GetGLExtensions() {
    GLint numExtensions;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

    std::unordered_set<std::string_view> extensions;

    extensions.reserve(numExtensions);

    for (int i = 0; i < numExtensions; i++) {
        extensions.emplace(reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i)));
    }

    return extensions;
}

void Window::MakeContextCurrent() { 
    glfwMakeContextCurrent(window);
}

bool Window::SupportsGLExtension(std::string_view str) {    
    static const auto extensions = GetGLExtensions();

    return extensions.contains(str);
}

void Window::FramebufferSizeCallback([[maybe_unused]] GLFWwindow* glfwwindow, int width, int height) noexcept {
    glViewport(0, 0, width, height);

    auto& window = Window::GetInstance();

    window.width = width;
    window.height = height;
    window.hasResized = true;
}

void Window::Create() {
    // TODO: check if gl_version is valid
    SetGLVersionString(glVersion);

    glfwSetErrorCallback([](int i, const char* desc) {
        spdlog::error("GLFW Error");
        spdlog::error("  Error code: {:#x}", i);
        spdlog::error("  Error description: {}", desc);
    });

    glfwInit();

    auto [gl_major_version, gl_minor_version, uses_gles] = glVersion;

    int gl_api = uses_gles ? GLFW_OPENGL_ES_API : GLFW_OPENGL_API;

    glfwWindowHint(GLFW_CLIENT_API, gl_api);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_major_version);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_minor_version);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
#ifndef __APPLE__
    glfwWindowHint(GLFW_SAMPLES, 8);
#endif

    window = glfwCreateWindow(width, height, "OpenGL", nullptr, nullptr); // Windowed

    if (!window) {
        throw WindowError{"Error creating window"};
    }

    glfwMakeContextCurrent(window);

    CaptureMouse();

    glfwSetKeyCallback(window, InputManager::KeyCallback);
    glfwSetCursorPosCallback(window, InputManager::MouseCallback);
    glfwSetMouseButtonCallback(window, InputManager::MouseButtonCallback);
    glfwSetFramebufferSizeCallback(window, Window::FramebufferSizeCallback);

    lastX = width / 2.0f;
    lastY = height / 2.0f;

    glfwSwapInterval(1);

    LoadGL();
}

void Window::InitGui() noexcept {
    ImGui_ImplGlfw_InitForOpenGL(window, true);
}

void Window::Destroy() noexcept { glfwDestroyWindow(window); }

void Window::LoadGL() {
    int load_result = (glVersion.is_gles ? gladLoadGLES2Loader : gladLoadGLLoader)((GLADloadproc)glfwGetProcAddress);

    if (load_result == 0) {
        throw WindowError{"Error loading OpenGL functions"};
    }
}

Window::~Window() {
    glfwTerminate();
}

Window& Window::GetInstance() noexcept {
    static Window w{};
    return w;
}

void Window::SwapBuffers() noexcept { glfwSwapBuffers(window); }

bool Window::ShouldClose() noexcept { return glfwWindowShouldClose(window); }

void Window::Close() noexcept { glfwSetWindowShouldClose(window, true); }

void Window::Resize(int width, int height) noexcept { glfwSetWindowSize(window, width, height); }

void Window::ReleaseMouse() noexcept { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }

void Window::CaptureMouse() noexcept { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }

std::string_view Window::VersionString() noexcept { return GLVersionString; }
