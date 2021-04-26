#include "Camera.h"

#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 target, float speed, glm::vec3 up) : 
    GameObject{position, {1, 1, 1}}, speed{speed} {
    // Set the front, right, up, etc vectors to their initial values
    UpdateVectors(glm::normalize(target - position), up);
    viewMat = glm::lookAt(position, target, vectors.upVector);
}

void Camera::SetPosition(glm::vec3 position) {
    GameObject::SetPosition(position);
    UpdateViewMatrix();
}

void Camera::Translate(glm::mat4 transform) {
    // Converts position to vec4, applies transform, then converts back to vec3
    SetPosition(glm::vec3{transform * glm::vec4(position, 1.0f)});
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

void Camera::Rotate(double pitch, double yaw) {
    double x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    double y = sin(glm::radians(pitch));
    double z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    UpdateVectors(glm::normalize(glm::vec3{x, y, z}), vectors.upVector);
    UpdateViewMatrix();
}
