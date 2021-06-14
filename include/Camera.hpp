#pragma once

#include "GameObject.hpp"

#include <Physics/Collision.hpp>

#include <cstdint>

#include <glm/glm.hpp>

// TODO: Add support for roll
class Camera : public GameObject {
    void UpdateViewMatrix() noexcept;
    void UpdateVectors(glm::vec3 frontVector, glm::vec3 upVector) noexcept;

    Physics::SimpleCubeCollider collider = {{}, 3, {}};
  public:
    Camera(glm::vec3 position, glm::vec3 target,
        float speed = 1.0f,
        glm::vec3 up = {0.0f, 1.0f, 0.0f} // y-axis defaults to up
        );

    // Runs every frame
    void Tick() override;

    void SetPosition(glm::vec3 position) override;

    void Rotate(double pitch, double yaw) noexcept;

    glm::mat4 viewMat{1.0f};

    enum class Direction : std::uint8_t { Forward, Backward, Right, Left, Up, Down };

    struct vectors {
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
};
