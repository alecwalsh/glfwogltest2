#include "InputManager.h"

#include <iostream>

#include "imgui.h"

extern float lastX, lastY;
extern double yaw, pitch;

std::array<InputManager::KeyState, GLFW_KEY_LAST> InputManager::keystates;
bool InputManager::mouseMoved = false;
bool InputManager::firstMouse = true;

void InputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
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

void InputManager::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (InputManager::GetInstance().mouseEnabled) {
        InputManager::mouseMoved = true;
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xSensitivity = 0.2f;
        float ySensitivity = 0.2f;

        auto deltaX = lastX - xpos;
        auto deltaY = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        yaw -= deltaX * xSensitivity;
        pitch += deltaY * ySensitivity;

        if (pitch > 89.0f) {
            pitch = 89.0f;
        }
        if (pitch < -89.0f) {
            pitch = -89.0f;
        }
    }
}

void InputManager::HandleInput() {
    glfwPollEvents();

    for (const auto& keybinding : key_bindings) {
        const auto& [keycode, desired_state, func] = keybinding;
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

void InputManager::DisableMouseInput() { mouseEnabled = false; }
void InputManager::EnableMouseInput() { mouseEnabled = true; }

void InputManager::DisableKeyboardInput() { keyboardEnabled = false; }
void InputManager::EnableKeyboardInput() { keyboardEnabled = true; }

void InputManager::DisableInput() {
    DisableMouseInput();
    DisableKeyboardInput();
}

void InputManager::EnableInput() {
    EnableMouseInput();
    EnableKeyboardInput();
}

InputManager& InputManager::GetInstance() {
    static InputManager im{};
    return im;
}
