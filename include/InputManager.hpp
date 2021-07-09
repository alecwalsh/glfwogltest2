#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <tuple>
#include <utility>
#include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// TODO: Allow events to fire on key release
// TODO: Handle modifier keys
// TODO: Input key as string, eg AddKeyBinding("Ctrl-A", []{})
class InputManager {
  public:
    // The mouse movement during the last frame
    double deltaX, deltaY;

    using Keycode = int;
    using MouseButton = int;

    enum class KeyState : std::uint8_t { NotPressed, InitialPress, RepeatPress, AnyPress };

    std::array<KeyState, GLFW_KEY_LAST> keyStates;
    std::array<KeyState, GLFW_MOUSE_BUTTON_LAST> mouseButtonStates;
    static inline bool mouseMoved = false;
    static inline bool firstMouse = true;

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept;
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) noexcept;
    static void MouseCallback(GLFWwindow* window, double xPos, double yPos) noexcept;

    template <typename F> void AddKeyBinding(InputManager::Keycode key, KeyState state, F&& f) {
        keyBindings.emplace_back(key, state, std::forward<F>(f));
    }

    template <typename F> void AddMouseButtonBinding(MouseButton button, KeyState state, F&& f) {
        mouseButtonBindings.emplace_back(button, state, std::forward<F>(f));
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
    std::vector<std::tuple<MouseButton, KeyState, std::function<void()>>> mouseButtonBindings;
};
