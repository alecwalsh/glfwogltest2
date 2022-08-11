#include "OrthographicCamera.hpp"

#include "TimeManager.hpp"

namespace GameEngine {

OrthographicCamera::OrthographicCamera(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
    : Camera{position, {1, 1, 1}}, direction{direction}, up{up} {
    name = "OrthographicCamera";
}

glm::mat4 OrthographicCamera::GetViewMatrix() const noexcept {
    // Look straight forward, with the positive x axis to the right of the screen and the positive y axis to the top of the screen
    return glm::lookAt(position, position + direction, up);
}

glm::mat4 OrthographicCamera::GetProjectionMatrix() const noexcept {
    float min = -10.0f;
    float max = 10.0f;

    return glm::ortho(min, max, min, max, 1.0f, 100.0f);
}

std::function<void()> OrthographicCamera::TranslateCamera(Direction d) {
    return [this, d] {
        glm::vec3 vec = {};

        switch (d) {
        case Direction::Forward:
            vec = direction;
            break;
        case Direction::Backward:
            vec = -direction;
            break;
        case Direction::Right:
            vec = glm::cross(direction, up);
            break;
        case Direction::Left:
            vec = -glm::cross(direction, up);
            break;
        case Direction::Up:
            vec = {0, 1, 0};
            break;
        case Direction::Down:
            vec = {0, -1, 0};
            break;
        default:
            break;
        }

        auto translation = speed * static_cast<float>(timeManager.deltaTime) * vec;

        ModifyPosition(translation);
    };
}

} // namespace GameEngine