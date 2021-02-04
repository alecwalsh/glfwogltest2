#pragma once

#include <cstdint>

#include <glm/glm.hpp>

// TODO: Make Camera a subclass of GameObject;  see TODO at top of GameObject.h
class Camera {
    void UpdateViewMatrix();
    void UpdateVectors(glm::vec3 frontVector, glm::vec3 upVector);

  public:
    Camera(glm::vec3 position, glm::vec3 target,
        float speed = 1.0f,
        glm::vec3 up = {0.0f, 1.0f, 0.0f} // y-axis defaults to up
        );

    void SetPosition(glm::vec3 position);
    void ModPosition(glm::mat4 transform);

    void Translate(glm::mat4 transform);
    void Rotate(double pitch, double yaw);

    glm::vec3 position{0.0f};
    //     glm::vec3 up{0.0f}; // Up vector for the whole scene

    glm::mat4 viewMat{1.0f};

    struct {
        using vec3 = glm::vec3;

		vec3 frontVector{0.0f};
        vec3 backVector{0.0f};
        vec3 rightVector{0.0f};
        vec3 leftVector{0.0f};
        vec3 upVector{0.0f};
        vec3 downVector{0.0f};
    } vectors;

    enum class Direction : std::uint8_t { Forward, Backward, Right, Left, Up, Down };

    float speed = 1.0f;
};
