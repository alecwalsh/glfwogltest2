#include "Camera.h"

Camera::Camera(glm::vec3 _position, glm::vec3 _target, glm::vec3 _up) : position(_position), up(_up) {
    viewMat = glm::lookAt(position, _target, up);

    cameraFront = glm::normalize(_target - position);
    auto& cf = cameraFront;
    auto x = cf[0];
    auto y = cf[1];
    auto z = cf[2];
    auto a = 0;
}

glm::vec3 Camera::UpVector() const {
    /*Calculates the up vector for the camera

    The nested cross products finds a vector perpendicular to cameraFront and in the same plane as cameraFront and up

    There's probably a better way to do this*/
    return glm::normalize(glm::cross(cameraFront, glm::cross(up, cameraFront)));
}

void Camera::SetPosition(glm::vec3 _position) {

    position = _position;
    UpdateViewMatrix();
}

void Camera::ModPosition(glm::mat4 _transform) {
    // Converts position to vec4, applies transform, then converts back to vec3
    position = glm::vec3(_transform * glm::vec4(position, 1.0f));
    UpdateViewMatrix();
}

void Camera::Translate(glm::mat4 _transform) {
    // Converts position to vec4, applies transform, then converts back to vec3
    position = glm::vec3(_transform * glm::vec4(position, 1.0f));

    UpdateViewMatrix();
}

void Camera::UpdateViewMatrix() { viewMat = glm::lookAt(position, position + cameraFront, up); }

void Camera::Rotate(float pitch, float yaw) {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(front);

    UpdateViewMatrix();
}
