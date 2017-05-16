#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Light.h"

class PointLight : public Light
{
public:
    glm::vec3 position;
    
	PointLight(glm::vec3 _position, glm::vec3 _diffuse, glm::vec3 _specular);
	virtual ~PointLight() override;
};
