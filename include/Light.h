#pragma once
#include <glm/glm.hpp>

class Light {
  public:
    glm::vec3 diffuse;
    glm::vec3 specular;

    Light(glm::vec3 _diffuse, glm::vec3 _specular);
    virtual ~Light();
};
