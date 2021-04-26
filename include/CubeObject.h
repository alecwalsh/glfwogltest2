#pragma once

#include "RenderableObject.h"

class CubeObject : public RenderableObject {
    float size = 1;   // TODO: Set this automatically
    float velocity = 0;
  public:
    float height = 25; // TODO: Set this automatically
    float RotSpeed = 0;
    
    CubeObject(MeshBase& mesh, ShaderProgram& shaderProgram, TextureManager& texman);
    
    // Runs every frame
    void Tick() override;
    void Draw(const Camera& camera) const override;
};
