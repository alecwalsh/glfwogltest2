#pragma once

#include "CameraBase.hpp"

#include <glm/glm.hpp>

namespace GameEngine {

class OrthographicCamera : public CameraBase {
    glm::vec3 direction;
    glm::vec3 up;

    float speed = 1.0f;
  public:
    OrthographicCamera(glm::vec3 position, glm::vec3 direction = {0, 0, -1}, glm::vec3 up = {0, 1, 0});

    // Runs every frame
    void Tick() noexcept override {}

    glm::mat4 GetViewMatrix() const noexcept override;
    glm::mat4 GetProjectionMatrix() const noexcept override;
    glm::vec3 GetFrontVector() const noexcept override { return direction; }

    // Create a lambda that translates the camera in a certain direction
    std::function<void()> TranslateCamera(CameraBase::Direction d) override;
};

} // namespace GameEngine