#pragma once

#include "Light.h"

// TODO: Fade out as distance from center increases
class SpotLight : public Light {
  public:
    glm::vec3 position;
    glm::vec3 direction;
    float cutoffAngle;

    SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular, float cutoffAngle);
};
