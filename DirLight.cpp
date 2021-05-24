#include "DirLight.hpp"

#include "glad/glad.h"

DirLight::DirLight(glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular)
    : Light{diffuse, specular, LightType::Directional}, direction{direction} {}

void DirLight::SetUniforms(GLuint program, std::size_t index) {
    auto getLightUniLoc = getLightUniLocGenerator(program, index);

    Light::SetUniforms(program, index);

    if (!active) return;

    glUniform3f(getLightUniLoc("direction"), direction.x, direction.y, direction.z);

    glUniform1i(getLightUniLoc("is_active"), active);
}
