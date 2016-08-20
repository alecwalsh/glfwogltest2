#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//TODO: Make Camera a subclass of GameObject;  see TODO at top of GameObject.h
class Camera {
public:
	Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up);
	
	void SetTransform(glm::mat4 _transform);
	void ModTransform(glm::mat4 _transform);

	glm::mat4 viewMat;
};