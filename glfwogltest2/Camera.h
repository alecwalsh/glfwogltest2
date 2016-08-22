#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//TODO: Make Camera a subclass of GameObject;  see TODO at top of GameObject.h
class Camera {
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 up; //Up vector for the whole scene

	void UpdateViewMatrix();
public:
	Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up);
	
	void SetPosition(glm::vec3 _position);
	void ModPosition(glm::mat4 _transform);

	void SetTarget(glm::vec3 _position);
	void ModTarget(glm::mat4 _transform);

	void Transform(glm::mat4 _transform);

	glm::mat4 viewMat;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp; //Up vector for the camera
};