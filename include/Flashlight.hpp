#pragma once

#include "SpotLight.hpp"

#include "Camera.hpp"

class Flashlight : public SpotLight {
    const GameEngine::Camera& camera;
  public:
    Flashlight(glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular, float cutoffAngleCos, const GameEngine::Camera& camera,
               bool active = true) noexcept;

    void SetUniforms(std::uint32_t program, std::size_t index) override;
};
