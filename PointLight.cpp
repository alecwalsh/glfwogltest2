#include "PointLight.h"

PointLight::PointLight(glm::vec3 _position, glm::vec3 _diffuse, glm::vec3 _specular)
    : Light(_diffuse, _specular), position(_position) {}

PointLight::~PointLight() {}
