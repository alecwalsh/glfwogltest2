#include "DirLight.h"

DirLight::DirLight(glm::vec3 direction_, glm::vec3 diffuse_, glm::vec3 specular_)
    : Light(diffuse_, specular_, LightType::Directional), direction(direction_) {}

DirLight::~DirLight() {}
