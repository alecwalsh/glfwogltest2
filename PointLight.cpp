#include "PointLight.h"

PointLight::PointLight(glm::vec3 position_, glm::vec3 diffuse_, glm::vec3 specular_)
    : Light(diffuse_, specular_, LightType::Point), position(position_) {}

PointLight::~PointLight() {}
