#pragma once
#include <glm/glm.hpp>

#include <memory>

#include "ShaderProgram.h"
#include "Mesh.h"

class GameObject
{
public:
	GameObject(Mesh& mesh, ShaderProgram& shaderprogram, glm::mat4 transform);
	virtual ~GameObject();
};

