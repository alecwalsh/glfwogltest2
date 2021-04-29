#include "UIManager.h"

#include "TimeManager.h"
#include "Window.h"

#include "imgui.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iterator>
#include <cfloat>

UIManager& UIManager::GetInstance() {
    thread_local UIManager uim{};
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

void UIManager::DrawStats() {
    unsigned int elapsedSeconds = static_cast<unsigned int>(timeManager.elapsedTime);

    ImGui::Begin("Stats");
    ImGui::Text("FPS: %u", displayFPS);
    ImGui::Text("Seconds: %u", elapsedSeconds);
    ImGui::Text("Frame %u", timeManager.frameCount);

    ImGui::End();
}

void UIManager::Draw() {
    BeginFrame();

    if (guiActive) {
        DrawOptional();
    }

    DrawAlwaysVisible();

    DrawMisc();

    EndFrame();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::DrawOptional() {
    ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)
    ImGui::InputText("", textBuffer, std::size(textBuffer));

    ImGui::End();
}

void UIManager::DrawMisc() {
    if (registeredFunctions.empty()) return;

    ImGui::Begin("Misc");

    while (!registeredFunctions.empty()) {
        registeredFunctions.front()();
        registeredFunctions.pop();
    }

    ImGui::End();
}

void UIManager::DrawAlwaysVisible() { DrawStats(); }

void UIManager::Initialize() {
    static bool firstCall = true;
    
    // Initialization only needs to be done once
    if (!firstCall) return;

    firstCall = false;

    const char* glsl_version = "#version 130";

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // ImGuiIO& io = ImGui::GetIO();
    // io.Fonts->AddFontFromFileTTF("data/fonts/Roboto-Medium.ttf", 16);

    Window::GetInstance().InitGui();
    ImGui_ImplOpenGL3_Init(glsl_version);

    using namespace std::chrono_literals;
    timeManager.AddTimer(Timer::Type::Repeat, 1s, [this] {
        displayFPS = static_cast<unsigned int>(timeManager.GetFPS());
    });
}

UIManager::~UIManager() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
