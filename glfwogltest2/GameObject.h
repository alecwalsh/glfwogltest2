#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "ShaderProgram.h"
#include "Mesh.h"

class GameObject
{
protected:
	Mesh& mesh;
	ShaderProgram& shaderProgram;
	glm::mat4 transform;
	GLuint vao;
	float& elapsedTime;
	float& deltaTime;
public:
	GameObject(Mesh& _mesh, ShaderProgram& _shaderProgram, glm::mat4 _transform, float& _elapsedTime, float& _deltaTime);
	virtual ~GameObject();
	GameObject(const GameObject& go);

	// Runs every frame
	virtual void Tick() = 0;
	// Renders the object
	void Draw();
	void SetupTextures();

	void SetTransform(glm::mat4 _transform);
	void ModTransform(glm::mat4 _transform);
};

