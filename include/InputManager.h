#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <tuple>
#include <utility>
#include <vector>

#include <GLFW/glfw3.h>

#define KEY(k) GLFW_KEY_##k

// TODO: Allow events to fire on key release
// TODO: Handle modifier keys
// TODO: Input key as string, eg AddKeyBinding("Ctrl-A", []{})
class InputManager {
  public:
    using keycode_t = int;

    enum class KeyState : std::uint8_t { NotPressed, InitialPress, RepeatPress, AnyPress };

    static std::array<KeyState, GLFW_KEY_LAST> keystates;
    static bool mouseMoved;

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

    static inline double min_pitch = -89.0;
    static inline double max_pitch = 89.0;

    template <typename F> void AddKeyBinding(InputManager::keycode_t key, KeyState state, F&& f);

    void HandleInput();

    [[nodiscard]] static InputManager& GetInstance();
    // Deleted to prevent copies
    InputManager(const InputManager&) = delete;
    void operator=(const InputManager&) = delete;

  private:
    InputManager() = default;
    ~InputManager() = default;

    std::vector<std::tuple<keycode_t, KeyState, std::function<void()>>> key_bindings;
};

template <typename F> void InputManager::AddKeyBinding(InputManager::keycode_t key, KeyState state, F&& f) {
    key_bindings.emplace_back(key, state, std::forward<F>(f));
}
