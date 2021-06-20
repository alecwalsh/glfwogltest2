#include "PointLight.hpp"

#include "glad/glad.h"

PointLight::PointLight(glm::vec3 position, glm::vec3 diffuse, glm::vec3 specular) noexcept
    : Light{diffuse, specular, LightType::Point}, position{position} {}

void PointLight::SetUniforms(std::uint32_t program, std::size_t index) {
    auto getLightUniLoc = GetLightUniLocGenerator(program, index);

    Light::SetUniforms(program, index);

    if (!active) return;

    glUniform3f(getLightUniLoc("position"), position.x, position.y, position.z);
}

