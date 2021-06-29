#include "InputManager.hpp"

#include "Window.hpp"

#include <iostream>

#include <imgui/imgui.h>

void InputManager::KeyCallback([[maybe_unused]] GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action,
    [[maybe_unused]] int mode) noexcept {
    if (InputManager::GetInstance().keyboardEnabled ||
        key == GLFW_KEY_U && !ImGui::GetIO().WantCaptureKeyboard) { // TODO: Don't hardcode key to hide UI
        auto& keystates = InputManager::keystates;

        // Don't want to use -1 as an array index
        if (key == GLFW_KEY_UNKNOWN) {
            std::cerr << "Unknown key pressed" << std::endl;
            return;
        }

        if (action == GLFW_PRESS) {
            if (keystates[key] == KeyState::NotPressed) {
                keystates[key] = KeyState::InitialPress;
            } else {
                keystates[key] = KeyState::RepeatPress;
            }
        } else if (action == GLFW_RELEASE) {
            keystates[key] = KeyState::NotPressed;
        }
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

void InputManager::HandleInput() noexcept {
    InputManager::mouseMoved = false;

    glfwPollEvents();

    for (const auto& [keycode, desired_state, func] : keyBindings) {
        auto& current_state = keystates[keycode];
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