#include "InputManager.h"

#include <iostream>

extern float lastX, lastY;
extern double yaw, pitch;

bool InputManager::keys[];
bool InputManager::mouseMoved;

void InputManager::key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    auto keys = InputManager::keys;
    
    // Don't want to use -1 as an array index
    if(key == GLFW_KEY_UNKNOWN) {
        std::cerr << "Unknown key pressed" << std::endl;
        return;
    }
    
    if (action == GLFW_PRESS) {
        keys[key] = true;
    } else if (action == GLFW_RELEASE) {
        keys[key] = false;
    }

    // When a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if (keys[GLFW_KEY_ESCAPE]) {
        glfwSetWindowShouldClose(window, GL_TRUE);
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
    
    for(const auto& keypair : key_bindings) {
        auto& key = keypair.first;
        auto& f = keypair.second;
        if(keys[key]) {
            f();
            //TODO: Remove this once key repeat is handled
            if(key == GLFW_KEY_R) {
                keys[key] = false;
            }
        }
    }
}

InputManager::InputManager() {
    
}

InputManager::~InputManager() {
    
}

InputManager& InputManager::GetInstance() {
    static InputManager im{};
    return im;
}
