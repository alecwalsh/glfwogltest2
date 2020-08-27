#pragma once

#include "Light.h"

class PointLight : public Light {
  public:
    glm::vec3 position;

    PointLight(glm::vec3 position, glm::vec3 diffuse, glm::vec3 specular);
};
