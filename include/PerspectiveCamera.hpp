#pragma once

#include "CameraBase.hpp"

#include <Physics/Collision.hpp>

#include <cstdint>

#include <glm/glm.hpp>

namespace GameEngine {

// TODO: Add support for roll
class PerspectiveCamera : public CameraBase {
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

    float speed = 1.0f;
  public:
    PerspectiveCamera(glm::vec3 position, glm::vec3 target,
        float speed = 1.0f,
        glm::vec3 up = {0.0f, 1.0f, 0.0f} // y-axis defaults to up
        );

    // Runs every frame
    void Tick() override;

    void SetPosition(glm::vec3 position) override;

    glm::mat4 GetViewMatrix() const noexcept override { return viewMat; }
    glm::mat4 GetProjectionMatrix() const noexcept override;
    glm::vec3 GetFrontVector() const noexcept override;

    void Rotate() noexcept;

    // Takes the mouse movement during the last frame and translates it into pitch and yaw
    void CalculatePitchAndYaw(double deltaX, double deltaY) noexcept;

    struct Vectors {
        using vec3 = glm::vec3;

        vec3 front = {};
        vec3 back = {};
        vec3 right = {};
        vec3 left = {};
        vec3 up = {};
        vec3 down = {};

        const vec3& operator[](Direction d) const noexcept;
        vec3& operator[](Direction d) noexcept;
    } vectors;

    // ParallelToGround means moving forward, back, left, and right results in movement parallel to the ground(pitch is 0)
    // Up and down movement simply moves up or down
    // With RelativeToCameraDirection, the camera's pitch applies to movement forward or backward
    enum class MovementStyle : std::uint8_t { ParallelToGround, RelativeToCameraDirection };

    MovementStyle movementStyle = MovementStyle::ParallelToGround;

    bool CheckCollision(glm::vec3 translation) const;

    // Create a lambda that translates the camera in a certain direction
    std::function<void()> TranslateCamera(CameraBase::Direction d) override;
};

}