#include "InputManager.hpp"

#include "Window.hpp"

#include <iostream>

#include <imgui/imgui.h>

void InputManager::KeyCallback([[maybe_unused]] GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action,
    [[maybe_unused]] int mods) noexcept {
    // Don't want to use -1 as an array index
    if (key == GLFW_KEY_UNKNOWN) {
        std::cerr << "Unknown key pressed" << std::endl;
        return;
    }

    auto& im = InputManager::GetInstance();

    Keycode hideUIKey = im.hideUIKey;

    bool shouldProcess = false;

    if (im.keyboardEnabled) {
        // If the keyboard is enabled, process the key press
        shouldProcess = true;
    } else {
        // If the keyboard is disabled and a text box is active, then the key press is consumed by the text box
        // However, if the keyboard is disabled, no text box is enabled, and the key is the Hide UI button,
        // then the key press should be processed like normal
        if (key == hideUIKey && !ImGui::GetIO().WantCaptureKeyboard) {
            shouldProcess = true;
        }
    }

    // Return early if we aren't processing the key press
    if (!shouldProcess) return;


    // Now we can process the key press
    auto& keyStates = im.keyStates;

    if (action == GLFW_PRESS) {
        if (keyStates[key] == KeyState::NotPressed) {
            keyStates[key] = KeyState::InitialPress;
        } else {
            keyStates[key] = KeyState::RepeatPress;
        }
    } else if (action == GLFW_RELEASE) {
        keyStates[key] = KeyState::NotPressed;
    }
}

void InputManager::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) noexcept {
    auto& mouseButtonStates = InputManager::GetInstance().mouseButtonStates;

    if (action == GLFW_PRESS) {
        if (mouseButtonStates[button] == KeyState::NotPressed) {
            mouseButtonStates[button] = KeyState::InitialPress;
        } else {
            mouseButtonStates[button] = KeyState::RepeatPress;
        }
    } else if (action == GLFW_RELEASE) {
        mouseButtonStates[button] = KeyState::NotPressed;
    }
}

// TODO: Mouse callback gets called when maximizing and restoring window, causing the camera to jump
void InputManager::MouseCallback([[maybe_unused]] GLFWwindow* window, double xPos, double yPos) noexcept {
    double& lastX = Window::GetInstance().lastX;
    double& lastY = Window::GetInstance().lastY;

    auto& im = InputManager::GetInstance();

    if(im.mouseEnabled) {
        InputManager::mouseMoved = true;
        if(InputManager::firstMouse) {
            lastX = xPos;
            lastY = yPos;
            firstMouse = false;
        }

        im.deltaX = lastX - xPos;
        im.deltaY = lastY - yPos;

        lastX = xPos;
        lastY = yPos;
    }
}

// Handles mouse or keyboard input, depending on the arguments
template<typename T, typename U>
static auto HandleInputGeneric(const T& bindings, U& states) {
    using KeyState = InputManager::KeyState;

    for (const auto& [keycode, desired_state, func] : bindings) {
        auto& current_state = states[keycode];
        if (desired_state == KeyState::AnyPress) {
            if (current_state == KeyState::InitialPress || current_state == KeyState::RepeatPress) {
                func();
            }
        } else if (current_state == desired_state) {
            func();
        }
        // After the initial press, state changes to RepeatPress
        if (current_state == KeyState::InitialPress) {
            current_state = KeyState::RepeatPress;
        }
    }
}

void InputManager::HandleInput() noexcept {
    InputManager::mouseMoved = false;

    glfwPollEvents();

    // Handle keyboard keys
    HandleInputGeneric(keyBindings, keyStates);

    // Handle mouse buttons
    HandleInputGeneric(mouseButtonBindings, mouseButtonStates);
}

void InputManager::DisableMouseInput() noexcept { mouseEnabled = false; }
void InputManager::EnableMouseInput() noexcept { mouseEnabled = true; }

void InputManager::DisableKeyboardInput() noexcept { keyboardEnabled = false; }
void InputManager::EnableKeyboardInput() noexcept { keyboardEnabled = true; }

void InputManager::DisableInput() noexcept {
    DisableMouseInput();
    DisableKeyboardInput();
}

void InputManager::EnableInput() noexcept {
    EnableMouseInput();
    EnableKeyboardInput();
}

InputManager& InputManager::GetInstance() noexcept {
    static InputManager im{};
    return im;
}