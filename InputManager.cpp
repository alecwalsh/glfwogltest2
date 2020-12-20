#include "InputManager.h"

#include <iostream>
#include <algorithm>

extern double lastX, lastY;
extern double yaw, pitch;

void InputManager::key_callback([[maybe_unused]] GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action,
                                [[maybe_unused]] int mode) {
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

void InputManager::mouse_callback([[maybe_unused]] GLFWwindow* window, double xpos, double ypos) {
    static bool firstMouse = true;

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

    pitch = std::clamp(pitch, InputManager::min_pitch, InputManager::max_pitch);
}

void InputManager::HandleInput() {
    glfwPollEvents();

    for (const auto& keybinding : key_bindings) {
        const auto & [ keycode, desired_state, func ] = keybinding;
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

InputManager& InputManager::GetInstance() {
    static InputManager im{};
    return im;
}
