#include "InputManager.h"

#include <iostream>

extern float lastX, lastY;
extern double yaw, pitch;

std::array<InputManager::KeyState, GLFW_KEY_LAST> InputManager::keystates;
bool InputManager::mouseMoved;

void InputManager::key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    auto& keystates = InputManager::keystates;
    
    // Don't want to use -1 as an array index
    if(key == GLFW_KEY_UNKNOWN) {
        std::cerr << "Unknown key pressed" << std::endl;
        return;
    }
    
    if (action == GLFW_PRESS) {
        if(keystates[key] == KeyState::NotPressed) {
            keystates[key] = KeyState::InitialPress;
        } else {
            keystates[key] = KeyState::RepeatPress;
        }
    } else if (action == GLFW_RELEASE) {
        keystates[key] = KeyState::NotPressed;
    }
}

void InputManager::mouse_callback(GLFWwindow *window, double xpos, double ypos) {
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

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }
}

void InputManager::HandleInput() {
    glfwPollEvents();
    
    for(const auto& keybinding : key_bindings) {
#if __cplusplus >= 201703L
        const auto& [keycode, desired_state, func] = keybinding;
#else
        const auto& keycode = std::get<0>(keybinding);
        const auto& desired_state = std::get<1>(keybinding);
        const auto& func = std::get<2>(keybinding);
#endif
        auto& current_state = keystates[keycode];
        if(desired_state == KeyState::AnyPress) {
            if(current_state == KeyState::InitialPress || current_state == KeyState::RepeatPress) {
                func();
            }
        } else if(current_state == desired_state) {
            func();
        }
        //After the initial press, state changes to RepeatPress
        if(current_state == KeyState::InitialPress) {
            current_state = KeyState::RepeatPress;
        }
    }
}

InputManager& InputManager::GetInstance() {
    static InputManager im{};
    return im;
}
