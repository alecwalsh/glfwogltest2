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

    // Update and Render additional Platform Windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void UIManager::DrawOptional() {
    ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)
    ImGui::InputText("##label", textBuffer, std::size(textBuffer)); // Need a dummy label to avoid an ID collision

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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    // io.Fonts->AddFontFromFileTTF("data/fonts/Roboto-Medium.ttf", 16);

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    #ifdef _WIN32
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    #endif

    auto& window = Window::GetInstance();

    window.InitGui();
    auto [gl_major_version, gl_minor_version, uses_gles] = window.glVersion;

    if(uses_gles) {
        assert(gl_major_version == 3); // Only support OpenGLES 3 and up
        // Dear ImGui doesn't detect the version correctly for 310 es and 320 es, causing a shader compilation failure
        // Use #version 300 es instead
        ImGui_ImplOpenGL3_Init("#version 300 es\n");
    } else {
        ImGui_ImplOpenGL3_Init(window.VersionString().data());
    }

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
