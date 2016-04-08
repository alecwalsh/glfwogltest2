#pragma once
#include <glm/glm.hpp>

#include <iostream>

#include "ShaderProgram.h"
#include "Mesh.h"
#include "RenderObject.h"

class GameObject
{
	Mesh& mesh;
	ShaderProgram& shaderProgram;
	glm::mat4 transform;
public:
	GameObject(Mesh& _mesh, ShaderProgram& _shaderProgram, glm::mat4 _transform);
	virtual ~GameObject();
	GameObject::GameObject(const GameObject& go);

	// Runs every frame
	void Tick(float elapsedTime);
	// Renders the object
	void Draw();
	void SetupTextures();
};

