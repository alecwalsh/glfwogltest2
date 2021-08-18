#pragma once

#include "RenderableObject.hpp"

#include <Physics/Collision.hpp>

#include <memory>
#include <concepts>

class CubeObject : public RenderableObject {
    glm::vec3 size = {1, 1, 1};

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

    template <std::derived_from<Physics::Collider> T>
    void SetCollider() {
        if constexpr (std::is_same_v<T, Physics::SphereCollider>) {
            collider = std::make_unique<T>(position, size.x, glm::vec3{});
        } else if constexpr (std::is_same_v<T, Physics::SimplePlaneCollider>) {
            collider = std::make_unique<T>(position.y);
        } else {
            collider = std::make_unique<T>(position, size, glm::vec3{});
        }
    }
};
