#include "Camera.h"

Camera::Camera(glm::vec3 eye, glm::vec3 center, glm::vec3 up)
{
	viewMat = glm::lookAt(eye, center, up);
}