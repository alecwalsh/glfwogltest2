#include "Light.hpp"

#include "glad/glad.h"

#include <sstream>

Light::Light(glm::vec3 diffuse, glm::vec3 specular, LightType type, bool active) noexcept
    : type{type}, diffuse{diffuse}, specular{specular}, active{active} {}
 
// Gets the uniform location for light struct members
GLint Light::GetLightUniLoc(const char* member, std::size_t index, GLuint program) {
    std::stringstream ss;
    ss << "lights[" << index << "]." << member;
    return glGetUniformLocation(program, ss.str().c_str());
}

void Light::SetUniforms(std::uint32_t program, std::size_t index) {
    auto getLightUniLoc = GetLightUniLocGenerator(program, index);

    glUniform1i(getLightUniLoc("is_active"), active);

    if (!active) return;

    glUniform1i(getLightUniLoc("type"), static_cast<GLint>(type));
    glUniform3f(getLightUniLoc("diffuse"), diffuse.r, diffuse.g, diffuse.b);
    glUniform3f(getLightUniLoc("specular"), specular.r, specular.g, specular.b);
}