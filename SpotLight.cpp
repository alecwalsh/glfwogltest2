#include "SpotLight.h"

SpotLight::SpotLight(glm::vec3 position_, glm::vec3 direction_, glm::vec3 diffuse_, glm::vec3 specular_, float cutoffAngle_)
    : Light(diffuse_, specular_, LightType::Spot), position(position_), direction(direction_), cutoffAngle(cutoffAngle_) {}

SpotLight::~SpotLight() {}
