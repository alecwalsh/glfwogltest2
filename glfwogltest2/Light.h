#pragma once
#include "GameObject.h"

class Light : public GameObject
{
public:
	Light(Mesh& _mesh, ShaderProgram& _shaderProgram, glm::mat4 _transform, float& _elapsedTime, float& _deltaTime);
	virtual ~Light() override;

	void Tick() override;
};

