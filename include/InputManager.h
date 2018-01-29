#pragma once

#include <functional>
#include <utility>
#include <tuple>
#include <vector>
#include <array>
#include <cstdint>

#include <GLFW/glfw3.h>

#define KEY(k) GLFW_KEY_ ## k

//TODO: Allow key binding to either repeat while the key is held down, or only fire once
//TODO: Allow events to fire on key release
//TODO: Handle modifier keys
//TODO: Input key as string, eg AddKeyBinding("Ctrl-A", []{})
class InputManager {
public:
    using keycode_t = int;
    
    enum class KeyState : std::uint8_t {
        NotPressed,
        InitialPress,
        RepeatPress,
        AnyPress
    };
    
    static std::array<KeyState, GLFW_KEY_LAST> keystates;
    static bool mouseMoved;
    
    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
    static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
    
    template <typename F>
    void AddKeyBinding(InputManager::keycode_t key, KeyState state, F&& f);
    
    void HandleInput();
    
    static InputManager& GetInstance();
    //Deleted to prevent copies
    InputManager(const InputManager&) = delete;
    void operator=(const InputManager&) = delete;
private:
    InputManager() = default;
    ~InputManager() = default;

    std::vector<std::tuple<keycode_t, KeyState, std::function<void()>>> key_bindings;
    
};

template <typename F>
void InputManager::AddKeyBinding(InputManager::keycode_t key, KeyState state, F&& f) {
    key_bindings.emplace_back(key, state, std::forward<F>(f));
}
