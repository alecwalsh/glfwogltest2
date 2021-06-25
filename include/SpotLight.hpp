#pragma once

#include "Light.hpp"

// TODO: Fade out as distance from center increases
class SpotLight : public Light {
  public:
    glm::vec3 position;
    glm::vec3 direction;
    float cutoffAngleCos; // Cosine of cutoff angle, to avoid acos in shader

    SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular, float cutoffAngleCos,
              bool active = true) noexcept;

    void SetUniforms(std::uint32_t program, std::size_t index) override;
};
