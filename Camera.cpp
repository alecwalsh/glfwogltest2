#include "Camera.hpp"

#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

#include "CubeObject.hpp"

static Physics::SimplePlaneCollider floorCollider = {0};

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

void Camera::UpdateViewMatrix() noexcept { viewMat = glm::lookAt(position, position + vectors.front, vectors.up); }

void Camera::UpdateVectors(glm::vec3 frontVector, glm::vec3 upVector) noexcept {
    // Calculates vectors from the perspective of the camera
    // This allows the camera to work no matter how it is moved and rotated
    vectors.front = glm::normalize(frontVector);
    vectors.back = -vectors.front;
    vectors.up = glm::normalize(upVector);
    vectors.down = -vectors.up;
    vectors.right = glm::normalize(glm::cross(vectors.front, vectors.up));
    vectors.left = -vectors.right;
}

void Camera::Rotate(double pitch, double yaw) noexcept {
    double x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    double y = sin(glm::radians(pitch));
    double z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    UpdateVectors({x, y, z}, vectors.up);
    UpdateViewMatrix();
}

void Camera::Tick() {
    // TODO: This sets collider.position, then SetPosition sets collider.position again, redundantly
    collider.ApplyCollision(floorCollider);
    SetPosition(collider.position);
}

const glm::vec3& Camera::Vectors::operator[](Direction d) const noexcept {
    // Uses array of pointers so returning a reference works right
    const vec3* vectorsArray[]{&front, &back, &right, &left, &up, &down};

    return *vectorsArray[static_cast<std::uint8_t>(d)];
}

glm::vec3& Camera::Vectors::operator[](Direction d) noexcept {
    // Uses array of pointers so returning a reference works right
    vec3* vectorsArray[]{&front, &back, &right, &left, &up, &down};

    return *vectorsArray[static_cast<std::uint8_t>(d)];
}

extern std::vector<CubeObject*> physicsObjects;

bool Camera::CheckCollision(glm::vec3 translation) const {
    auto newCollider = collider;
    newCollider.position += translation;

    bool anyCollide = false;

    for (auto go2 : physicsObjects) {
        if (!go2->HasCollider())
            continue;

        auto& collider2 = go2->GetCollider();
        if (&collider == &collider2)
            continue;
        if (!newCollider.SupportsCollisionWith(collider2))
            continue;

        if (newCollider.CollidesWith(collider2)) {
            anyCollide = true;
        }
    }

    return anyCollide;
}