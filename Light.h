#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Light
{
public:
	glm::vec3 diffuse;
	glm::vec3 specular;

	Light(glm::vec3 _diffuse, glm::vec3 _specular);
	virtual ~Light();
};
