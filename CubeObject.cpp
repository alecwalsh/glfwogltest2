#include "CubeObject.h"

#include "TimeManager.h"

constexpr float earthGravity = 9.81f;

bool collidesWithFloor(float height, float floorHeight, float size) { 
    return (height - size/2) <= floorHeight; 
}

float calculateDistance(float& velocity) {
    float acceleration = earthGravity;

    float time = timeManager.deltaTime;

    velocity += acceleration * time;

    float distance = velocity * time + (acceleration * time * time) / 2;

    return distance;
}

void CubeObject::Tick() {
    glm::mat4 rotation{1.0f}, translation{1.0f}, scaling{1.0f};

    float rotationAmount = deltaTime * RotSpeed * glm::radians(180.0f);

    rotation = glm::rotate(rotation, rotationAmount, glm::vec3{0.0f, 1.0f, 0.0f});

    float distance;

    float floorHeight = 0;

    if (collidesWithFloor(height, floorHeight, size)) {
        // Is currently colliding with the floor
        distance = 0;
        velocity = 0;
    } else {
        distance = calculateDistance(velocity);
        // Is not currently colliding with the floor
        if (collidesWithFloor(height - distance, floorHeight, size)) {
            // The new height will collide with the floor
            // Set distance so that the new height is exactly at the floor
            distance = height - size / 2 - floorHeight;
        }

        height -= distance;
    }

    translation = glm::translate(translation, glm::vec3{0.0f, -distance, 0.0f});

    this->ModTransform(translation * rotation * scaling);
}

void CubeObject::Draw(const Camera& camera) const {
    // Set material properties
    GLint matAmbientLoc = glGetUniformLocation(shaderProgram.shaderProgram, "material.ambient");
    GLint matDiffuseLoc = glGetUniformLocation(shaderProgram.shaderProgram, "material.diffuse");
    GLint matSpecularLoc = glGetUniformLocation(shaderProgram.shaderProgram, "material.specular");
    GLint matShineLoc = glGetUniformLocation(shaderProgram.shaderProgram, "material.shininess");

    glUniform3f(matAmbientLoc, material.ambient.r, material.ambient.g, material.ambient.b);
    glUniform3f(matDiffuseLoc, material.diffuse.r, material.diffuse.g, material.diffuse.b);
    glUniform3f(matSpecularLoc, material.specular.r, material.specular.g, material.specular.b);
    glUniform1f(matShineLoc, material.shininess);

    // Sets up cameraPos uniform then calls base class method
    GLint uniCameraPos = glGetUniformLocation(shaderProgram.shaderProgram, "cameraPos");
    glUniform3f(uniCameraPos, camera.position.x, camera.position.y, camera.position.z);

    GLint timeUniform = glGetUniformLocation(shaderProgram.shaderProgram, "time");
    glUniform1f(timeUniform, elapsedTime);

    GameObject::Draw(camera);
}

CubeObject::CubeObject(MeshBase& mesh, ShaderProgram& shaderProgram, glm::mat4 transform, TextureManager& texman)
    : GameObject(mesh, shaderProgram, transform, texman) {
    // Sets up material properties for the cube
    material.ambient = glm::vec3{1.0f, 0.5f, 0.31f};
    material.diffuse = glm::vec3{1.0f, 0.5f, 0.31f};
    material.specular = glm::vec3{1.0f, 1.0f, 1.0f};
    material.shininess = 32.0f;
}
