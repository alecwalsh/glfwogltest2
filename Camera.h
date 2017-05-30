#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


//TODO: Make Camera a subclass of GameObject;  see TODO at top of GameObject.h
class Camera {
	void UpdateViewMatrix();
public:
	Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up);
	
	glm::vec3 UpVector() const;

	void SetPosition(glm::vec3 _position);
	void ModPosition(glm::mat4 _transform);

	void Translate(glm::mat4 _transform);
	void Rotate(float pitch, float yaw);

	glm::vec3 position;
	glm::vec3 up; //Up vector for the whole scene

	glm::mat4 viewMat;
	glm::vec3 cameraFront;
};
