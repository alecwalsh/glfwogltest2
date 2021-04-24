#pragma once
#include "GameObject.h"

class CubeObject : public GameObject {
  public:
    float RotSpeed = 0;

    CubeObject(MeshBase& mesh, ShaderProgram& shaderProgram, glm::mat4 transform, TextureManager& texman);
    
    // Runs every frame
    void Tick() override;
    void Draw(const Camera& camera) const override;
};
