#include "PointLight.hpp"

#include "glad/glad.h"

PointLight::PointLight(glm::vec3 position, glm::vec3 diffuse, glm::vec3 specular)
    : Light{diffuse, specular, LightType::Point}, position{position} {}

void PointLight::SetUniforms(GLuint program, std::size_t index) {
    auto getLightUniLoc = getLightUniLocGenerator(program, index);

    Light::SetUniforms(program, index);

    if (!active) return;

    glUniform3f(getLightUniLoc("position"), position.x, position.y, position.z);
}

