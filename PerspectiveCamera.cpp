#include "PerspectiveCamera.hpp"

#include "World.hpp"
#include "InputManager.hpp"
#include "TimeManager.hpp"

#include <cmath>
#include <algorithm>

static Physics::SimplePlaneCollider floorCollider = {0};

namespace GameEngine {

PerspectiveCamera::PerspectiveCamera(glm::vec3 position, glm::vec3 target, float speed, glm::vec3 up)
    : Camera{position, {1, 1, 1}}, speed{speed} {
    name = "PerspectiveCamera";
    collider.position = position;
    // Set the front, right, up, etc vectors to their initial values
    UpdateVectors(target - position, up);
    UpdateViewMatrix();

    // Sets pitch and yaw based on the cameraFront vector;  this prevents the camera from jumping when moving the mouse
    // for the first time
    // This is just the inverse of the code in Camera::Rotate
    const auto& cf = vectors.front;

    pitch = glm::degrees(asin(cf.y));
    yaw = glm::degrees(acos(cf.x / cos(asin(cf.y))));

    if (cf.z < 0) {
        yaw = -yaw;
    }
}

void PerspectiveCamera::SetPosition(glm::vec3 position) {
    GameObject::SetPosition(position);
    collider.position = position;

    UpdateViewMatrix();
}

void PerspectiveCamera::UpdateViewMatrix() noexcept {
    viewMat = glm::lookAt(position, position + vectors.front, vectors.up);
}

void PerspectiveCamera::UpdateVectors(glm::vec3 frontVector, glm::vec3 upVector) noexcept {
    // Calculates vectors from the perspective of the camera
    // This allows the camera to work no matter how it is moved and rotated
    vectors.front = glm::normalize(frontVector);
    vectors.back = -vectors.front;
    vectors.up = glm::normalize(upVector);
    vectors.down = -vectors.up;
    vectors.right = glm::normalize(glm::cross(vectors.front, vectors.up));
    vectors.left = -vectors.right;
}

void PerspectiveCamera::Rotate() noexcept {
    double x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    double y = sin(glm::radians(pitch));
    double z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    UpdateVectors({x, y, z}, vectors.up);
    UpdateViewMatrix();
}

void PerspectiveCamera::CalculatePitchAndYaw(double deltaX, double deltaY) noexcept {
    yaw -= deltaX * xSensitivity;
    pitch += deltaY * ySensitivity;

    pitch = std::clamp(pitch, minPitch, maxPitch);

    Rotate();
}

void PerspectiveCamera::Tick() {
    if (InputManager::mouseMoved) {
        auto& im = InputManager::GetInstance();
        CalculatePitchAndYaw(im.deltaX, im.deltaY);
    }

    // TODO: This sets collider.position, then SetPosition sets collider.position again, redundantly
    //collider.ApplyCollision(floorCollider);
    SetPosition(collider.position);
}

const glm::vec3& PerspectiveCamera::Vectors::operator[](Direction d) const noexcept {
    // Uses array of pointers so returning a reference works right
    const vec3* vectorsArray[]{&front, &back, &right, &left, &up, &down};

    return *vectorsArray[static_cast<std::uint8_t>(d)];
}

glm::vec3& PerspectiveCamera::Vectors::operator[](Direction d) noexcept {
    // Uses array of pointers so returning a reference works right
    vec3* vectorsArray[]{&front, &back, &right, &left, &up, &down};

    return *vectorsArray[static_cast<std::uint8_t>(d)];
}

bool PerspectiveCamera::CheckCollision(glm::vec3 translation) const {
    auto newCollider = collider;
    newCollider.position += translation;

    bool anyCollide = false;

    for (auto& go : GameEngine::world->gameObjects) {
        if (auto* go2 = dynamic_cast<GameEngine::CubeObject*>(go.get()); go2->HasCollider()) {
            auto& collider2 = go2->GetCollider();

            if (&collider == &collider2) {
                assert(false);
            }
            if (!newCollider.SupportsCollisionWith(collider2))
                continue;

            if (newCollider.CollidesWith(collider2)) {
                anyCollide = true;
            }
        }
    }

    return anyCollide;
}

std::function<void()> PerspectiveCamera::TranslateCamera(Direction d) {
    return [this, d] {
        auto vec = vectors[d];

        if (movementStyle == MovementStyle::ParallelToGround) {
            if (!(d == Direction::Up || d == Direction::Down)) {
                vec.y = 0;

                vec = glm::normalize(vec);
            }
        }

        auto translation = speed * static_cast<float>(timeManager.deltaTime) * vec;

        bool willCollide = CheckCollision(translation);

        if (!willCollide) {
            ModifyPosition(translation);
        }
    };
}

glm::mat4 PerspectiveCamera::GetProjectionMatrix() const noexcept {
    return glm::perspective(glm::radians(45.0f), width / height, 1.0f, 100.0f);
}
glm::vec3 PerspectiveCamera::GetFrontVector() const noexcept {
    return vectors.front;
}

}