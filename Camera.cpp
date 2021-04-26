#include "Camera.h"

#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 target, float speed, glm::vec3 up) : 
    GameObject{position, {1, 1, 1}}, speed{speed} {
    // Set the front, right, up, etc vectors to their initial values
    UpdateVectors(glm::normalize(target - position), up);
    viewMat = glm::lookAt(position, target, vectors.up);
}

void Camera::SetPosition(glm::vec3 position) {
    GameObject::SetPosition(position);
    UpdateViewMatrix();
}

void Camera::Translate(glm::mat4 transform) {
    // Converts position to vec4, applies transform, then converts back to vec3
    SetPosition(glm::vec3{transform * glm::vec4(position, 1.0f)});
}

void Camera::UpdateViewMatrix() { viewMat = glm::lookAt(position, position + vectors.front, vectors.up); }

void Camera::UpdateVectors(glm::vec3 frontVector, glm::vec3 upVector) {
    // Calculates vectors from the perspective of the camera
    // This allows the camera to work no matter how it is moved and rotated
    vectors.front = frontVector;
    vectors.back = -vectors.front;
    vectors.up = upVector;
    vectors.down = -vectors.up;
    vectors.right = glm::normalize(glm::cross(vectors.front, vectors.up));
    vectors.left = -vectors.right;
}

void Camera::Rotate(double pitch, double yaw) {
    double x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    double y = sin(glm::radians(pitch));
    double z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    UpdateVectors(glm::normalize(glm::vec3{x, y, z}), vectors.up);
    UpdateViewMatrix();
}

const glm::vec3& Camera::vectors::operator[](Direction d) {
    // Uses array of pointers so returning a reference works right
    vec3* vectorsArray[]{&front, &back, &right, &left, &up, &down};

    return *vectorsArray[static_cast<std::uint8_t>(d)];
}