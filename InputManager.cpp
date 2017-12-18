#include "InputManager.h"

bool InputManager::keys[];

void InputManager::key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    auto keys = InputManager::keys;
    
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

void InputManager::AddKeyBinding(key_t key, std::function<void()> f) {
    key_bindings.emplace_back(key, f);
}

void InputManager::HandleInput() {
    for(auto& [key, f] : key_bindings) {
        if(keys[key]) {
            f();
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
