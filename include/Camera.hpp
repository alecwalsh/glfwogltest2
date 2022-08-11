#pragma once

#include "GameObject.hpp"

#include <glm/glm.hpp>

#include <cstdint>
#include <functional>

namespace GameEngine {

class Camera : public GameObject {
  protected:
    float width;
    float height;
  public:
    enum class Direction : std::uint8_t { Forward, Backward, Right, Left, Up, Down };

    Camera(glm::vec3 position, glm::vec3 scale) : GameObject{position, scale} {}

    virtual glm::mat4 GetViewMatrix() const = 0;
    virtual glm::mat4 GetProjectionMatrix() const = 0;
    virtual glm::vec3 GetFrontVector() const = 0;

    void SetWidth(float width) { this->width = width; }
    void SetHeight(float height) { this->height = height; }

    // Create a lambda that translates the camera in a certain direction
    virtual std::function<void()> TranslateCamera(Direction d) = 0;
};

} // namespace GameEngine