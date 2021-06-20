#pragma once

#include "SpotLight.hpp"

#include "Camera.hpp"

class Flashlight : public SpotLight {
    const Camera& camera;
  public:
    Flashlight(glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular, float cutoffAngleCos, const Camera& camera) noexcept;

    void SetUniforms(std::uint32_t program, std::size_t index) override;
};
