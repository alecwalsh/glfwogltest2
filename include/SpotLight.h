#pragma once

#include "Light.h"

// TODO: Fade out as distance from center increases
class SpotLight : public Light {
  public:
    glm::vec3 position;
    glm::vec3 direction;
    float cutoffAngle;

    SpotLight(glm::vec3 position_, glm::vec3 direction_, glm::vec3 diffuse_, glm::vec3 specular_, float cutoffAngle);
    virtual ~SpotLight() override;
};
