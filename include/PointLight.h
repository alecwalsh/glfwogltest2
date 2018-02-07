#pragma once

#include "Light.h"

class PointLight : public Light {
  public:
    glm::vec3 position;

    PointLight(glm::vec3 position_, glm::vec3 diffuse_, glm::vec3 specular_);
    virtual ~PointLight() override;
};
