#pragma once

#include <functional>
#include <utility>
#include <vector>

#include <GLFW/glfw3.h>

//TODO: Allow key binding to either repeat while the key is held down, or only fire once
//TODO: Allow events to fire on key release
//TODO: Handle modifier keys
class InputManager {
public:
    using keycode_t = int;
    
    static bool keys[GLFW_KEY_LAST];
    static bool mouseMoved;
    
    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
    static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
    
    template <typename F>
    void AddKeyBinding(keycode_t key, F&& f);
    
    void HandleInput();
    
    static InputManager& GetInstance();
    //Deleted to prevent copies
    InputManager(const InputManager&) = delete;
    void operator=(const InputManager&) = delete;
private:
    InputManager();
    ~InputManager();

    std::vector<std::pair<keycode_t, std::function<void()>>> key_bindings;
    
};

template <typename F>
void InputManager::AddKeyBinding(InputManager::keycode_t key, F&& f) {
    key_bindings.emplace_back(key, std::forward<F>(f));
}
