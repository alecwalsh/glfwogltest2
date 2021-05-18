#pragma once

#include "RenderableObject.h"

#include <Physics/Collision.hpp>

#include <memory>

class CubeObject : public RenderableObject {
    float size = 1;

    std::unique_ptr<Physics::Collider> collider;
  public:
    float RotSpeed = 0;
    
    CubeObject(MeshBase& mesh, ShaderProgram& shaderProgram, TextureManager& texman);

    void SetPosition(glm::vec3 position) override;

    void SetScale(glm::vec3 scale) override;
    
    // Runs every frame
    void Tick() override;
    void Draw(const Camera& camera) const override;

    template <typename T> void SetCollider() { 
        collider = std::make_unique<T>(position, size, glm::vec3{});
    }
};
