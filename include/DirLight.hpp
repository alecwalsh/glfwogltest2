#pragma once

#include "Light.hpp"

class DirLight : public Light {
  public:
    glm::vec3 direction;

    DirLight(glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular) noexcept;

    void SetUniforms(GLuint program, std::size_t index) override;
};
