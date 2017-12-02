#pragma once

#include <GLFW/glfw3.h>

//TODO: Move GLFW callbacks into this class
class InputManager {
InputManager();
~InputManager();
public:
    //TODO: find out what size this needs to be
    static bool keys[1024];
    
    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
    
    static InputManager& GetInstance();
    //Deleted to prevent copies
    InputManager(const InputManager&) = delete;
    void operator=(const InputManager&) = delete;
};
