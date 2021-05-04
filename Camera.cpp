#include "Camera.h"

#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

#include "Collision.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 target, float speed, glm::vec3 up) : 
    GameObject{position, {1, 1, 1}}, speed{speed} {
    name = "Camera";
    collider.position = position;
    // Set the front, right, up, etc vectors to their initial values
    UpdateVectors(target - position, up);
    UpdateViewMatrix();
}

void Camera::SetPosition(glm::vec3 position) {
    GameObject::SetPosition(position);
    collider.position = position;

    UpdateViewMatrix();
}

void Camera::UpdateViewMatrix() { viewMat = glm::lookAt(position, position + vectors.front, vectors.up); }

void Camera::UpdateVectors(glm::vec3 frontVector, glm::vec3 upVector) {
    // Calculates vectors from the perspective of the camera
    // This allows the camera to work no matter how it is moved and rotated
    vectors.front = glm::normalize(frontVector);
    vectors.back = -vectors.front;
    vectors.up = glm::normalize(upVector);
    vectors.down = -vectors.up;
    vectors.right = glm::normalize(glm::cross(vectors.front, vectors.up));
    vectors.left = -vectors.right;
}

void Camera::Rotate(double pitch, double yaw) {
    double x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    double y = sin(glm::radians(pitch));
    double z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    UpdateVectors({x, y, z}, vectors.up);
    UpdateViewMatrix();
}

void Camera::Tick() {
    ModifyPosition(Physics::getTranslation(velocityVector, collider));
}

const glm::vec3& Camera::vectors::operator[](Direction d) {
    // Uses array of pointers so returning a reference works right
    vec3* vectorsArray[]{&front, &back, &right, &left, &up, &down};

    return *vectorsArray[static_cast<std::uint8_t>(d)];
}