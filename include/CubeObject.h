#pragma once
#include "GameObject.h"

class CubeObject : public GameObject {
  public:
    float RotSpeed = 0;
    CubeObject(Mesh& mesh, ShaderProgram& shaderProgram, glm::mat4 transform, float& elapsedTime, float& deltaTime,
               TextureManager& texman);
    virtual ~CubeObject() override;
    void Tick() override;
    void Draw(const Camera& camera) const override;

    void LuaRegister(LuaScript& L) override;
};
