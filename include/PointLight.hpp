#pragma once

#include "Light.hpp"

class PointLight : public Light {
  public:
    glm::vec3 position;

    PointLight(glm::vec3 position, glm::vec3 diffuse, glm::vec3 specular) noexcept;

    void SetUniforms(std::uint32_t program, std::size_t index) override;
};
