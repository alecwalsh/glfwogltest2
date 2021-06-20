#include "DirLight.hpp"

#include "glad/glad.h"

DirLight::DirLight(glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular) noexcept
    : Light{diffuse, specular, LightType::Directional}, direction{direction} {}

void DirLight::SetUniforms(std::uint32_t program, std::size_t index) {
    auto getLightUniLoc = GetLightUniLocGenerator(program, index);

    Light::SetUniforms(program, index);

    if (!active) return;

    glUniform3f(getLightUniLoc("direction"), direction.x, direction.y, direction.z);
}
