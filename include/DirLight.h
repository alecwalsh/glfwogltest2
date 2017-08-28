#pragma once

#include "Light.h"

class DirLight : public Light {
  public:
    glm::vec3 direction;

    DirLight(glm::vec3 _direction, glm::vec3 _diffuse, glm::vec3 _specular);
    virtual ~DirLight() override;
};
