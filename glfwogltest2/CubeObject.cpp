#include "CubeObject.h"

// Runs every frame
void CubeObject::Tick()
{
	GLint uniTime = glGetUniformLocation(shaderProgram.shaderProgram, "time");
	GLint uniModel = glGetUniformLocation(shaderProgram.shaderProgram, "model");

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

	std::cout << "Elapsed time:" << elapsedTime << std::endl;
}

CubeObject::CubeObject(Mesh& _mesh, ShaderProgram& _shaderProgram, glm::mat4 _transform, float& _elapsedTime, float& _deltaTime) : GameObject(_mesh, _shaderProgram, _transform, _elapsedTime, _deltaTime)
{

}