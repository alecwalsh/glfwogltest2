#include "CubeObject.h"

// Runs every frame
void CubeObject::Tick()
{
	//TODO: move glGetUniformLocation calls outside of tick function; they only need to be called once per shader
	GLint uniTime = glGetUniformLocation(shaderProgram.shaderProgram, "time");

	float time = elapsedTime;

	bool ascending = ((int)time / 5) % 2 == 0;

	time = time / 5;
	if (ascending) {
		glUniform1f(uniTime, time - floor(time));
	}
	else {
		glUniform1f(uniTime, 1 - (time - floor(time)));
	}

	glm::mat4 rotation, translation, scaling;

	rotation = glm::rotate(
		rotation,
		deltaTime * glm::radians(180.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	translation = glm::translate(translation, glm::vec3(0.0f, 0.0f, 0.0f));
	scaling = glm::scale(scaling, glm::vec3(1.0f, 1.0f, 1.0f));

	//this->ModTransform(translation * rotation * scaling);

	//std::cout << "Elapsed time:" << elapsedTime << std::endl;
}

void CubeObject::Draw(Camera camera)
{
	//Set material properties
	GLint matAmbientLoc = glGetUniformLocation(shaderProgram.shaderProgram, "material.ambient");
	GLint matDiffuseLoc = glGetUniformLocation(shaderProgram.shaderProgram, "material.diffuse");
	GLint matSpecularLoc = glGetUniformLocation(shaderProgram.shaderProgram, "material.specular");
	GLint matShineLoc = glGetUniformLocation(shaderProgram.shaderProgram, "material.shininess");

	glUniform3f(matAmbientLoc, material.ambient.r, material.ambient.g, material.ambient.b);
	glUniform3f(matDiffuseLoc, material.diffuse.r, material.diffuse.g, material.diffuse.b);
	glUniform3f(matSpecularLoc, material.specular.r, material.specular.g, material.specular.b);
	glUniform1f(matShineLoc, material.shininess);


	//Sets up cameraPos uniform then calls base class method
	GLint uniCameraPos = glGetUniformLocation(shaderProgram.shaderProgram, "cameraPos");
	glUniform3f(uniCameraPos, camera.position.x, camera.position.y, camera.position.z);

	GameObject::Draw(camera);
}

CubeObject::CubeObject(Mesh& _mesh, ShaderProgram& _shaderProgram, glm::mat4 _transform, float& _elapsedTime, float& _deltaTime) : GameObject(_mesh, _shaderProgram, _transform, _elapsedTime, _deltaTime)
{
	//Sets up material properties for the cube
	material.ambient = glm::vec3(1.0f, 0.5f, 0.31f);
	material.diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
	material.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	material.shininess = 32.0f;
}

CubeObject::~CubeObject()
{
}