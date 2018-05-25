#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// TODO: Make Camera a subclass of GameObject;  see TODO at top of GameObject.h
class Camera {
    void UpdateViewMatrix();
    void UpdateVectors(glm::vec3 frontVector, glm::vec3 upVector);

  public:
    Camera(glm::vec3 _position, glm::vec3 _target, glm::vec3 _up);

    void SetPosition(glm::vec3 _position);
    void ModPosition(glm::mat4 _transform);

    void Translate(glm::mat4 _transform);
    void Rotate(float pitch, float yaw);

    glm::vec3 position{0.0f};
    //     glm::vec3 up{0.0f}; // Up vector for the whole scene

    glm::mat4 viewMat{1.0f};

    struct vectors {
        using vec3 = glm::vec3;
        vec3 rightVector{0.0f};
        vec3 leftVector{0.0f};
        vec3 frontVector{0.0f};
        vec3 backVector{0.0f};
        vec3 upVector{0.0f};
        vec3 downVector{0.0f};
    } vectors;

    enum class Direction { Forward, Backward, Left, Right, Up, Down };
};
