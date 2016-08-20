#pragma once
#include "GameObject.h"

class CubeObject : public GameObject
{
public:
	CubeObject(Mesh& _mesh, ShaderProgram& _shaderProgram, glm::mat4 _transform, float& _elapsedTime, float& _deltaTime);
	virtual void Tick();
};