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

    glm::vec3 position;
//     glm::vec3 up; // Up vector for the whole scene

    glm::mat4 viewMat;
    
    struct vectors {
        using vec3 = glm::vec3;
        vec3 rightVector;
        vec3 leftVector;
        vec3 frontVector;
        vec3 backVector;
        vec3 upVector;
        vec3 downVector;
    } vectors;
    
    enum class Direction {
        Forward,
        Backward,
        Left,
        Right,
        Up,
        Down
    };
};
