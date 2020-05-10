#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// TODO: Make Camera a subclass of GameObject;  see TODO at top of GameObject.h
class Camera {
    void UpdateViewMatrix();
    void UpdateVectors(glm::vec3 frontVector, glm::vec3 upVector);

  public:
    Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up);

    void SetPosition(glm::vec3 position);
    void ModPosition(glm::mat4 transform);

    void Translate(glm::mat4 transform);
    void Rotate(float pitch, float yaw);

    void SetInitialPitchYaw(double& pitch, double& yaw);

    glm::vec3 position{0.0f};

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
