#pragma once

#include "RenderableObject.h"

class CubeObject : public RenderableObject {
    float height = 5; // TODO: Set this automatically
    float size = 1;   // TODO: Set this automatically
    float velocity = 0;
  public:
    float RotSpeed = 0;
    
    CubeObject(MeshBase& mesh, ShaderProgram& shaderProgram, glm::mat4 transform, TextureManager& texman);
    
    // Runs every frame
    void Tick() override;
    void Draw(const Camera& camera) const override;
};
