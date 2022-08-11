#include "Flashlight.hpp"

#include <glad/glad.h>

Flashlight::Flashlight(glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular, float cutoffAngleCos,
                       const GameEngine::Camera& camera, bool active) noexcept
    : SpotLight{{}, direction, diffuse, specular, cutoffAngleCos, active}, camera{camera} {}

void Flashlight::SetUniforms(std::uint32_t program, std::size_t index) {
    auto getLightUniLoc = GetLightUniLocGenerator(program, index);

    Light::SetUniforms(program, index);

    if (!active) return;

    auto cameraPos = camera.GetPosition();
    // Set the position and direction to the camera's, like a flashlight
    glUniform3f(getLightUniLoc("position"), cameraPos.x, cameraPos.y, cameraPos.z);

    auto frontVec = camera.GetFrontVector();
    glUniform3f(getLightUniLoc("direction"), frontVec.x, frontVec.y, frontVec.z);

    glUniform1f(getLightUniLoc("cutoffAngleCos"), cutoffAngleCos);
}
