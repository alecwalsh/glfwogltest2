#pragma once

#include "Light.hpp"

class DirLight : public Light {
  public:
    glm::vec3 direction;

    DirLight(glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular) noexcept;

    void SetUniforms(std::uint32_t program, std::size_t index) override;
};
