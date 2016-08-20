#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up)
{
	viewMat = glm::lookAt(position, target, up);
}


void Camera::SetTransform(glm::mat4 _transform)
{
	viewMat = _transform;
}


void Camera::ModTransform(glm::mat4 _transform)
{
	viewMat *= _transform;
}