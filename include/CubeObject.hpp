#pragma once

#include "RenderableObject.hpp"

#include <Physics/Collision.hpp>

#include <memory>
#include <concepts>

namespace GameEngine {

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
    void Draw(const GameEngine::CameraBase& camera) const override;

    bool HasCollider() const noexcept;
    Physics::Collider& GetCollider() const;
    void SetCollider(std::unique_ptr<Physics::Collider> collider);

    template <std::derived_from<Physics::Collider> T> void SetCollider() {
        if constexpr (std::is_same_v<T, Physics::SphereCollider>) {
            SetCollider(std::make_unique<T>(position, size.x, glm::vec3{}));
        } else if constexpr (std::is_same_v<T, Physics::SimplePlaneCollider>) {
            SetCollider(std::make_unique<T>(position.y));
        } else {
            SetCollider(std::make_unique<T>(position, size, glm::vec3{}));
        }
    }
};

} // namespace GameEngine