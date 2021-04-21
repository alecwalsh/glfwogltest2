#pragma once

#include "SpotLight.h"

#include "Camera.h"

class Flashlight : public SpotLight {
    const Camera& camera;
  public:
    Flashlight(glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular, float cutoffAngleCos, const Camera& camera);

    void SetUniforms(GLuint program, std::size_t index) override;
};
