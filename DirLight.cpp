#include "DirLight.h"

DirLight::DirLight(glm::vec3 _direction, glm::vec3 _diffuse, glm::vec3 _specular)
    : Light(_diffuse, _specular), direction(_direction) {}

DirLight::~DirLight() {}
