#pragma once

#include "GameObject.hpp"

#include <glm/glm.hpp>

namespace GameEngine {

class CameraBase : public GameObject {
  protected:
    float width;
    float height;
  public:
    CameraBase(glm::vec3 position, glm::vec3 scale) : GameObject{position, scale} {}

    virtual glm::mat4 GetViewMatrix() const = 0;
    virtual glm::mat4 GetProjectionMatrix() const = 0;

    void SetWidth(float width) { this->width = width; }
    void SetHeight(float height) { this->height = height; }
};

} // namespace GameEngine