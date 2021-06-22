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
    using Keycode = int;

    enum class KeyState : std::uint8_t { NotPressed, InitialPress, RepeatPress, AnyPress };

    static inline std::array<KeyState, GLFW_KEY_LAST> keystates;
    static inline bool mouseMoved = false;
    static inline bool firstMouse = true;

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) noexcept;
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos) noexcept;

    static inline double min_pitch = -89.0;
    static inline double max_pitch = 89.0;

    template <typename F> void AddKeyBinding(InputManager::Keycode key, KeyState state, F&& f) {
        keyBindings.emplace_back(key, state, std::forward<F>(f));
    }

    void HandleInput() noexcept;

    void DisableMouseInput() noexcept;
    void EnableMouseInput() noexcept;
    
    void DisableKeyboardInput() noexcept;
    void EnableKeyboardInput() noexcept;
    
    void DisableInput() noexcept;
    void EnableInput() noexcept;

    [[nodiscard]] static InputManager& GetInstance() noexcept;
    // Deleted to prevent copies
    InputManager(const InputManager&) = delete;
    void operator=(const InputManager&) = delete;

  private:
    InputManager() = default;
    ~InputManager() = default;

    bool mouseEnabled = true;
    bool keyboardEnabled = true;

    std::vector<std::tuple<Keycode, KeyState, std::function<void()>>> keyBindings;
};
