#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <tuple>
#include <utility>
#include <vector>

#include <GLFW/glfw3.h>

// TODO: Allow events to fire on key release
// TODO: Handle modifier keys
// TODO: Input key as string, eg AddKeyBinding("Ctrl-A", []{})
class InputManager {
  public:
    using keycode_t = int;

    enum class KeyState : std::uint8_t { NotPressed, InitialPress, RepeatPress, AnyPress };

    static inline std::array<KeyState, GLFW_KEY_LAST> keystates;
    static inline bool mouseMoved = false;
    static inline bool firstMouse = true;

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

    static inline double min_pitch = -89.0;
    static inline double max_pitch = 89.0;

    template <typename F> void AddKeyBinding(InputManager::keycode_t key, KeyState state, F&& f) {
        key_bindings.emplace_back(key, state, std::forward<F>(f));
    }

    void HandleInput();

    void DisableMouseInput();
    void EnableMouseInput();
    
    void DisableKeyboardInput();
    void EnableKeyboardInput();
    
    void DisableInput();
    void EnableInput();

    [[nodiscard]] static InputManager& GetInstance();
    // Deleted to prevent copies
    InputManager(const InputManager&) = delete;
    void operator=(const InputManager&) = delete;

  private:
    InputManager() = default;
    ~InputManager() = default;

    bool mouseEnabled = true;
    bool keyboardEnabled = true;

    std::vector<std::tuple<keycode_t, KeyState, std::function<void()>>> key_bindings;
};
