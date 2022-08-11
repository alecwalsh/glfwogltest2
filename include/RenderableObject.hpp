#pragma once

#include "GameObject.hpp"

#include "Light.hpp"
#include "Camera.hpp"
#include "MeshBase.hpp"
#include "ShaderManager.hpp"
#include "TextureManager.hpp"

#include <span>
#include <memory>

namespace GameEngine {

class RenderableObject : public GameObject {
  protected:
    MeshBase& mesh;

    struct {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;
    } material = {};

    std::uint32_t vao;
    TextureManager& texman;

  public:
    using GameObject::GameObject;

    std::string texture_name = "container";
    std::string spec_texture_name = "container_specular";

    RenderableObject(MeshBase& mesh, ShaderProgram& shaderProgram, TextureManager& texman);
    RenderableObject(const RenderableObject&);

    // Renders the object
    virtual void Draw(const Camera& camera) const;

    void BindTextures() const;

    ShaderProgram& shaderProgram;

    void RenderObject(std::span<const std::unique_ptr<Light>> lights, const Camera& camera);
};

} // namespace GameEngine