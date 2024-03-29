#include "SpotLight.hpp"

#include "glad/glad.h"

SpotLight::SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular,
                     float cutoffAngleCos, bool active) noexcept
    : Light{diffuse, specular, LightType::Spot, active}, position{position}, direction{direction},
      cutoffAngleCos{cutoffAngleCos} {}

void SpotLight::SetUniforms(std::uint32_t program, std::size_t index) {
    auto getLightUniLoc = GetLightUniLocGenerator(program, index);

    Light::SetUniforms(program, index);

    if (!active) return;

    glUniform3f(getLightUniLoc("position"), position.x, position.y, position.z);
    glUniform3f(getLightUniLoc("direction"), direction.x, direction.y, direction.z);

    glUniform1f(getLightUniLoc("cutoffAngleCos"), cutoffAngleCos);
}
