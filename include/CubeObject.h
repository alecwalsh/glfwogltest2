#pragma once
#include "GameObject.h"

class CubeObject : public GameObject {
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
