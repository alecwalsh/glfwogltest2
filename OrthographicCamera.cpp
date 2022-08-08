#include "OrthographicCamera.hpp"

namespace GameEngine {

OrthographicCamera::OrthographicCamera(glm::vec3 position, glm::vec3 direction)
    : CameraBase{position, {1, 1, 1}}, direction{direction} {
    name = "OrthographicCamera";
}

glm::mat4 OrthographicCamera::GetViewMatrix() const noexcept {
    // Look straight forward, with the positive x axis to the right of the screen and the positive y axis to the top of the screen
    return glm::lookAt(position, position + direction, {0, 1, 0});
}

glm::mat4 OrthographicCamera::GetProjectionMatrix() const noexcept {
    float min = -10.0f;
    float max = 10.0f;

    return glm::ortho(min, max, min, max, 1.0f, 100.0f);
}

} // namespace GameEngine