#pragma once

#include "Light.h"

class DirLight : public Light {
  public:
    glm::vec3 direction;

    DirLight(glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular);
    virtual ~DirLight() override;
};
