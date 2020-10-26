#include "UIManager.h"

#include "Window.h"

#include <iterator>

#include "imgui.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

UIManager& UIManager::GetInstance() {
    static UIManager uim{};
    return uim;
}

void UIManager::BeginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    if (!guiActive) {
        ImGui::GetIO().MousePos = {-FLT_MAX, -FLT_MAX};
    }

    ImGui::NewFrame();
}

void UIManager::EndFrame() {
    ImGui::Render();
}

void UIManager::Draw() {
    BeginFrame();

    ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)
    ImGui::InputText("", textBuffer, std::size(textBuffer));

    ImGui::End();

    EndFrame();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

UIManager::UIManager() {
    const char* glsl_version = "#version 130";

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    Window::GetInstance().InitGui();
    ImGui_ImplOpenGL3_Init(glsl_version);
}

UIManager::~UIManager() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
