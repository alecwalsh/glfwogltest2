#pragma once

#include "RenderableObject.h"

#include "Collision.hpp"

class CubeObject : public RenderableObject {
    float size = 1;

    glm::vec3 velocityVector = {};

    Physics::SphereCollider collider = {{}, 0.5f};
    Physics::SimpleCubeCollider cubeCollider = {{}, 1};
  public:
    float RotSpeed = 0;
    
    CubeObject(MeshBase& mesh, ShaderProgram& shaderProgram, TextureManager& texman);

    void SetPosition(glm::vec3 position) override;

    void SetScale(glm::vec3 scale) override;
    
    // Runs every frame
    void Tick() override;
    void Draw(const Camera& camera) const override;
};
