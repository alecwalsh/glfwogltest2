#pragma once

#include "CameraBase.hpp"

#include <glm/glm.hpp>

namespace GameEngine {

class OrthographicCamera : public CameraBase {
    glm::vec3 direction;
  public:
    OrthographicCamera(glm::vec3 position, glm::vec3 direction = {0, 0, -1});

    // Runs every frame
    void Tick() noexcept override {}

    glm::mat4 GetViewMatrix() const noexcept override;
    glm::mat4 GetProjectionMatrix() const noexcept override;
};

} // namespace GameEngine