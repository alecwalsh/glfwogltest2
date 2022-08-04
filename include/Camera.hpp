#pragma once

#include "CameraBase.hpp"
#include "TimeManager.hpp"

#include <Physics/Collision.hpp>

#include <cstdint>

#include <glm/glm.hpp>

// TODO: Add support for roll
class Camera : public GameEngine::CameraBase {
    void UpdateViewMatrix() noexcept;
    void UpdateVectors(glm::vec3 frontVector, glm::vec3 upVector) noexcept;

    Physics::SimpleCubeCollider collider = {{}, {1.75, 3, 1.75}, {}};

    double xSensitivity = 0.2f;
    double ySensitivity = 0.2f;

    double minPitch = -89.0;
    double maxPitch = 89.0;

    double pitch;
    double yaw;

    glm::mat4 viewMat{1.0f};
  public:
    Camera(glm::vec3 position, glm::vec3 target,
        float speed = 1.0f,
        glm::vec3 up = {0.0f, 1.0f, 0.0f} // y-axis defaults to up
        );

    // Runs every frame
    void Tick() override;

    void SetPosition(glm::vec3 position) override;

    glm::mat4 GetViewMatrix() const noexcept override { return viewMat; }
    glm::mat4 GetProjectionMatrix() const noexcept override;

    void Rotate() noexcept;

    // Takes the mouse movement during the last frame and translates it into pitch and yaw
    void CalculatePitchAndYaw(double deltaX, double deltaY) noexcept;

    enum class Direction : std::uint8_t { Forward, Backward, Right, Left, Up, Down };

    struct Vectors {
        using vec3 = glm::vec3;

        vec3 front{0.0f};
        vec3 back{0.0f};
        vec3 right{0.0f};
        vec3 left{0.0f};
        vec3 up{0.0f};
        vec3 down{0.0f};

        const vec3& operator[](Direction d) const noexcept;
        vec3& operator[](Direction d) noexcept;
    } vectors;

    float speed = 1.0f;

    // ParallelToGround means moving forward, back, left, and right results in movement parallel to the ground(pitch is 0)
    // Up and down movement simply moves up or down
    // With RelativeToCameraDirection, the camera's pitch applies to movement forward or backward
    enum class MovementStyle : std::uint8_t { ParallelToGround, RelativeToCameraDirection };

    MovementStyle movementStyle = MovementStyle::ParallelToGround;

    bool CheckCollision(glm::vec3 translation) const;

    // Create a lambda that translates the camera in a certain direction
    auto TranslateCamera(Camera::Direction d) {
        return [this, d] {
            // TODO: Get key bindings from files
            // TODO: Figure out how to use control key

            auto vec = vectors[d];

            if (movementStyle == MovementStyle::ParallelToGround) {
                if (!(d == Camera::Direction::Up || d == Camera::Direction::Down)) {
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
    };
};
