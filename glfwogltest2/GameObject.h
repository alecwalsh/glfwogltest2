#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "ShaderProgram.h"
#include "Mesh.h"

class GameObject
{
	Mesh& mesh;
	ShaderProgram& shaderProgram;
	glm::mat4 transform;
	GLuint vao;
public:
	GameObject(Mesh& _mesh, ShaderProgram& _shaderProgram, glm::mat4 _transform);
	virtual ~GameObject();
	GameObject(const GameObject& go);

	// Runs every frame
	void Tick();
	// Renders the object
	void Draw();
	void SetupTextures();

	void SetTransform(glm::mat4 _transform);
};

