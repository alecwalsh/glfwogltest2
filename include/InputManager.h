#pragma once

#include <functional>
#include <utility>
#include <vector>

#include <GLFW/glfw3.h>

//TODO: Move GLFW callbacks into this class
class InputManager {
public:
    using key_t = int;
    
    static bool keys[GLFW_KEY_LAST];
    
    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
    
    void AddKeyBinding(key_t key, std::function<void()>);
    
    void HandleInput();
    
    static InputManager& GetInstance();
    //Deleted to prevent copies
    InputManager(const InputManager&) = delete;
    void operator=(const InputManager&) = delete;
private:
    InputManager();
    ~InputManager();

    std::vector<std::pair<key_t, std::function<void()>>> key_bindings;
    
};
