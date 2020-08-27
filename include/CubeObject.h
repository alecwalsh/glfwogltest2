#pragma once
#include "GameObject.h"

class CubeObject : public GameObject {
  public:
    float RotSpeed = 0;
    CubeObject(MeshBase& mesh, ShaderProgram& shaderProgram, glm::mat4 transform, float& elapsedTime, float& deltaTime,
               TextureManager& texman);
    void Tick() override;
    void Draw(const Camera& camera) const override;
};
