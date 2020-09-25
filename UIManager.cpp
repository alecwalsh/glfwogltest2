#include "UIManager.h"

#include "imgui.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

GLFWwindow* UIManager::window;

UIManager& UIManager::GetInstance() {
    static UIManager uim{};
    return uim;
}

UIManager::UIManager() {
    const char* glsl_version = "#version 130";

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

UIManager::~UIManager() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
