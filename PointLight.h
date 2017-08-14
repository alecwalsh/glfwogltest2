#pragma once

#include "Light.h"

class PointLight : public Light {
  public:
    glm::vec3 position;

    PointLight(glm::vec3 _position, glm::vec3 _diffuse, glm::vec3 _specular);
    virtual ~PointLight() override;
};
