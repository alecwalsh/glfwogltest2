#include "Flashlight.h"

Flashlight::Flashlight(glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular, float cutoffAngleCos,
                       const Camera& camera)
    : SpotLight{glm::vec3{0.0f}, direction, diffuse, specular, cutoffAngleCos}, camera{camera} {}

void Flashlight::SetUniforms(GLuint program, std::size_t index) {
    auto getLightUniLoc = getLightUniLocGenerator(program, index);

    Light::SetUniforms(program, index);

    if (!active) return;

    // Set the position and direction to the camera's, like a flashlight
    glUniform3f(getLightUniLoc("position"), camera.position.x, camera.position.y, camera.position.z);
    glUniform3f(getLightUniLoc("direction"), camera.vectors.frontVector.x, camera.vectors.frontVector.y,
                camera.vectors.frontVector.z);

    glUniform1f(getLightUniLoc("cutoffAngleCos"), cutoffAngleCos);
}
