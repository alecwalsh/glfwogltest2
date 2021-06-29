#include "UIManager.hpp"

#include "TimeManager.hpp"
#include "InputManager.hpp"
#include "Window.hpp"

#include <imgui/imgui.h>

#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <iterator>
#include <cfloat>

UIManager& UIManager::GetInstance() noexcept {
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
    // Initialization only needs to be done once
    if (initialized) return;

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

    initialized = true;
}

void UIManager::Destroy() {
    if (initialized) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        initialized = false;
    }
}

UIManager::~UIManager() {
    Destroy();
}

void UIManager::ToggleUI() noexcept {
    auto& window = Window::GetInstance();
    auto& im = InputManager::GetInstance();

    if (window.showCursor) {
        window.CaptureMouse();
        im.EnableInput();

        UIManager::GetInstance().guiActive = false;

        window.showCursor = false;
    } else {
        InputManager::firstMouse = true; // Prevent camera from jumping to previous cursor location

        window.ReleaseMouse();
        im.DisableInput();

        UIManager::GetInstance().guiActive = true;

        window.showCursor = true;
    }
}
