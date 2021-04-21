#include "Light.h"

#include "glad/glad.h"

#include <sstream>

Light::Light(glm::vec3 diffuse, glm::vec3 specular, LightType type)
    : type{type}, diffuse{diffuse}, specular{specular} {}
 
// Gets the uniform location for light struct members
GLint Light::getLightUniLoc(const char* member, std::size_t index, GLuint program) {
    std::stringstream ss;
    ss << "lights[" << index << "]." << member;
    return glGetUniformLocation(program, ss.str().c_str());
}

void Light::SetUniforms(GLuint program, std::size_t index) {
    auto getLightUniLoc = getLightUniLocGenerator(program, index);

    glUniform1i(getLightUniLoc("is_active"), active);

    if (!active) return;

    glUniform1i(getLightUniLoc("type"), static_cast<int>(type));
    glUniform3f(getLightUniLoc("diffuse"), diffuse.r, diffuse.g, diffuse.b);
    glUniform3f(getLightUniLoc("specular"), specular.r, specular.g, specular.b);
}