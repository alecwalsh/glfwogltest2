#pragma once

#include "RenderableObject.hpp"

#include <Physics/Collision.hpp>

#include <memory>

#if __cpp_concepts >= 201907L
#include <concepts>
#endif

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

    bool HasCollider();
    Physics::Collider& GetCollider();
    void SetCollider(std::unique_ptr<Physics::Collider> collider);

    #if __cpp_concepts >= 201907L
    template <std::derived_from<Physics::Collider> T>
    #else
    template <typename T>
    #endif
    void SetCollider() {
        collider = std::make_unique<T>(position, size, glm::vec3{});
    }
};
