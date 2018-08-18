#include "CubeObject.h"

// Runs every frame
void CubeObject::Tick() {
    glm::mat4 rotation{1.0f}, translation{1.0f}, scaling{1.0f};

    rotation = glm::rotate(rotation, deltaTime * RotSpeed * glm::radians(180.0f), glm::vec3{0.0f, 1.0f, 0.0f});

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

    GameObject::Draw(camera);
}

CubeObject::CubeObject(Mesh& mesh, ShaderProgram& shaderProgram, glm::mat4 transform, float& elapsedTime,
                       float& deltaTime, TextureManager& texman)
    : GameObject(mesh, shaderProgram, transform, elapsedTime, deltaTime, texman) {
    // Sets up material properties for the cube
    material.ambient = glm::vec3{1.0f, 0.5f, 0.31f};
    material.diffuse = glm::vec3{1.0f, 0.5f, 0.31f};
    material.specular = glm::vec3{1.0f, 1.0f, 1.0f};
    material.shininess = 32.0f;
}

CubeObject::~CubeObject() {}
