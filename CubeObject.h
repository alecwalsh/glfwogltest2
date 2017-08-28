#pragma once
#include <lua.hpp>

#include "GameObject.h"

class CubeObject : public GameObject {
private:
    lua_State* L;
    bool usesLua = false;
public:
    CubeObject(Mesh &_mesh, ShaderProgram &_shaderProgram, glm::mat4 _transform, float &_elapsedTime, float &_deltaTime,
                TextureManager &_texman);
    virtual ~CubeObject() override;
    void Tick() override;
    void TickLua();
    void Draw(Camera camera) const override;
};
