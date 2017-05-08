#include "Light.h"



Light::Light(glm::vec3 _diffuse, glm::vec3 _specular)
{
	diffuse = _diffuse;
	specular = _specular;
}


Light::~Light()
{
}
