#include "CubeObject.h"

#include "Collision.h"

void CubeObject::Tick() {
    ModifyPosition(Physics::getTranslation(velocity, height, size));
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
    auto cameraPos = camera.GetPosition();
    glUniform3f(uniCameraPos, cameraPos.x, cameraPos.y, cameraPos.z);

    GLint timeUniform = glGetUniformLocation(shaderProgram.shaderProgram, "time");
    glUniform1f(timeUniform, static_cast<float>(elapsedTime));

    RenderableObject::Draw(camera);
}

CubeObject::CubeObject(MeshBase& mesh, ShaderProgram& shaderProgram, TextureManager& texman)
    : RenderableObject{mesh, shaderProgram, texman} {
    // Sets up material properties for the cube
    material.ambient = glm::vec3{1.0f, 0.5f, 0.31f};
    material.diffuse = glm::vec3{1.0f, 0.5f, 0.31f};
    material.specular = glm::vec3{1.0f, 1.0f, 1.0f};
    material.shininess = 32.0f;
}


void CubeObject::SetPosition(glm::vec3 position) {
    RenderableObject::SetPosition(position);
    this->height = position.y;
}

void CubeObject::SetScale(glm::vec3 scale) {
    RenderableObject::SetScale(scale);
    this->size = scale.y;
}