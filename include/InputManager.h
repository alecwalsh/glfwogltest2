#pragma once

#include <functional>
#include <utility>
#include <vector>

#include <GLFW/glfw3.h>

extern bool mouseMoved;

//TODO: Move GLFW mouse callback into this class
//TODO: Allow key binding to either repeat while the key is held down, or only fire once
//TODO: Allow events to fire on key release
//TODO: Handle modifier keys
class InputManager {
public:
    using key_t = int;
    
    static bool keys[GLFW_KEY_LAST];
    
    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
    static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
    
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
