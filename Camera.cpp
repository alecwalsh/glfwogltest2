#include "Camera.h"

Camera::Camera(glm::vec3 _position, glm::vec3 _target, glm::vec3 _up) : position(_position) {
    // Set the front, right, up, etc vectors to their initial values
    UpdateVectors(glm::normalize(_target - position), _up);
    viewMat = glm::lookAt(position, _target, vectors.upVector);
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

void Camera::UpdateViewMatrix() { viewMat = glm::lookAt(position, position + vectors.frontVector, vectors.upVector); }

void Camera::UpdateVectors(glm::vec3 frontVector, glm::vec3 upVector) {
    // Calculates vectors from the perspective of the camera
    // This allows the camera to work no matter how it is moved and rotated
    vectors.frontVector = frontVector;
    vectors.backVector = -vectors.frontVector;
    vectors.upVector = upVector;
    vectors.downVector = -vectors.upVector;
    vectors.rightVector = glm::normalize(glm::cross(vectors.frontVector, vectors.upVector));
    vectors.leftVector = -vectors.rightVector;
}

void Camera::Rotate(float pitch, float yaw) {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    vectors.frontVector = glm::normalize(front);

    UpdateViewMatrix();
    UpdateVectors(vectors.frontVector, vectors.upVector);
}
