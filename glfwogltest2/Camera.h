#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
	Camera(glm::vec3 eye, glm::vec3 center, glm::vec3 up);
	
	glm::mat4 viewMat;
};