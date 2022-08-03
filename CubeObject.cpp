#include "CubeObject.hpp"

#include "glad/glad.h"

#include "TimeManager.hpp"
#include "UIManager.hpp"

#include <imgui/imgui.h>

#include <stdexcept>

namespace GameEngine {

static Physics::SimplePlaneCollider floorCollider = {0};
static Physics::SphereCollider sphere1Collider = {{0, 0, 2.0f}, 1, {}};

void CubeObject::Tick() {
    if (!collider) return;

    Physics::Collider* otherCollider = &floorCollider;

    if (name == "sphere2") {
        otherCollider = &sphere1Collider;

        uiManager.AddToUI([vel = collider->velocity, pos = collider->position] {
            ImGui::Text("Velocity %.2f, %.2f, %.2f", vel.x, vel.y, vel.z);

            ImGui::Text("Position: %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);
        });
    }

    if (collider->SupportsCollisionWith(*otherCollider)) {
        collider->ApplyCollision(*otherCollider);
    }

    SetPosition(collider->position);
}

void CubeObject::Draw(const GameEngine::CameraBase& camera) const {
    GLuint sp = shaderProgram.shaderProgram;

    // Set material properties
    GLint matAmbientLoc = glGetUniformLocation(sp, "material.ambient");
    GLint matDiffuseLoc = glGetUniformLocation(sp, "material.diffuse");
    GLint matSpecularLoc = glGetUniformLocation(sp, "material.specular");
    GLint matShineLoc = glGetUniformLocation(sp, "material.shininess");

    glUniform3f(matAmbientLoc, material.ambient.r, material.ambient.g, material.ambient.b);
    glUniform3f(matDiffuseLoc, material.diffuse.r, material.diffuse.g, material.diffuse.b);
    glUniform3f(matSpecularLoc, material.specular.r, material.specular.g, material.specular.b);
    glUniform1f(matShineLoc, material.shininess);

    // Sets up cameraPos uniform then calls base class method
    GLint uniCameraPos = glGetUniformLocation(sp, "cameraPos");
    auto cameraPos = camera.GetPosition();
    glUniform3f(uniCameraPos, cameraPos.x, cameraPos.y, cameraPos.z);

    GLint timeUniform = glGetUniformLocation(sp, "time");
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

    if (collider) {
        collider->position = position;
    }
}

void CubeObject::SetScale(glm::vec3 scale) {
    RenderableObject::SetScale(scale);
    this->size = scale;

    if (collider) {
        this->collider->size = scale;
    }
}

bool CubeObject::HasCollider() const noexcept { return static_cast<bool>(collider); }

Physics::Collider& CubeObject::GetCollider() const {
    if (!collider) {
        throw std::runtime_error{"Attempted to access nonexistent collider"};
    }
    return *collider;
}

void CubeObject::SetCollider(std::unique_ptr<Physics::Collider> collider) {
    this->collider = std::move(collider);
}

} // namespace GameEngine