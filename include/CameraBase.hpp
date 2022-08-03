#pragma once

#include "GameObject.hpp"

#include <glm/glm.hpp>

namespace GameEngine {

class CameraBase : public GameObject {
  public:
    CameraBase(glm::vec3 position, glm::vec3 scale) : GameObject{position, scale} {}

    virtual const glm::mat4& GetViewMatrix() const = 0;
};

} // namespace GameEngine