#include "Camera.h"

Camera::Camera(glm::vec3 _position, glm::vec3 _target, glm::vec3 _up): position(_position), target(_target), up(_up)
{
	viewMat = glm::lookAt(position, target, up);

	cameraFront = glm::normalize(target - position);

	/*Calculates the up vector for the camera

	The nested cross products finds a vector perpendicular to cameraFront and in the same plane as cameraFront and up

	There's probably a better way to do this*/
	cameraUp = glm::normalize(glm::cross(cameraFront, glm::cross(up, cameraFront)));
}


void Camera::SetPosition(glm::vec3 _position)
{

	position = _position;
	UpdateViewMatrix();
}

void Camera::ModPosition(glm::mat4 _transform)
{
	//Converts position to vec4, applies transform, then converts back to vec3
	position = glm::vec3(_transform * glm::vec4(position, 1.0f));
	UpdateViewMatrix();
}

void Camera::SetTarget(glm::vec3 _target)
{
	target = _target;
	UpdateViewMatrix();
}

void Camera::ModTarget(glm::mat4 _transform)
{
	//Converts target to vec4, applies transform, then converts back to vec3
	target = glm::vec3(_transform * glm::vec4(target, 1.0f));
	UpdateViewMatrix();
}

void Camera::Transform(glm::mat4 _transform)
{
	//Converts position and target to vec4, applies transform, then converts them back to vec3
	//Only updates view matrix after applying transform to both
	//Changes both position and target so the camera doesn't rotate as the camera moves
	position = glm::vec3(_transform * glm::vec4(position, 1.0f));
	target = glm::vec3(_transform * glm::vec4(target, 1.0f));
	
	UpdateViewMatrix();
}

void Camera::UpdateCamera()
{
	//viewMat = glm::lookAt(position, target, up);
	//viewMat = glm::lookAt(position, position + cameraFront, cameraUp);
}


void Camera::UpdateViewMatrix()
{
	//viewMat = glm::lookAt(position, target, up);
	viewMat = glm::lookAt(position, position + cameraFront, cameraUp);
}

void Camera::Rotate(float pitch, float yaw) {
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	
	cameraFront = glm::normalize(front);

	cameraUp = glm::normalize(glm::cross(cameraFront, glm::cross(up, cameraFront)));

	UpdateViewMatrix();
}