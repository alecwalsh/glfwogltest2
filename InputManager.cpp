#include "InputManager.h"

bool InputManager::keys[1024];

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

InputManager::InputManager() {
    
}

InputManager::~InputManager() {
    
}

InputManager& InputManager::GetInstance() {
    static InputManager im{};
    return im;
}
