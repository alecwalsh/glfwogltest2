#pragma once

#include "RenderableObject.h"

class CubeObject : public RenderableObject {
    float height = 0;
    float size = 1;
    float velocity = 0;
  public:
    float RotSpeed = 0;
    
    CubeObject(MeshBase& mesh, ShaderProgram& shaderProgram, TextureManager& texman);

    void SetPosition(glm::vec3 position) override {
        RenderableObject::SetPosition(position);
        this->height = position.y;
    }

    void SetScale(glm::vec3 scale) override {
        RenderableObject::SetScale(scale);
        this->size = scale.y;
    }
    
    // Runs every frame
    void Tick() override;
    void Draw(const Camera& camera) const override;
};
