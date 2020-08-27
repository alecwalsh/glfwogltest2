#include "DirLight.h"

DirLight::DirLight(glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular)
    : Light{diffuse, specular, LightType::Directional}, direction(direction) {}
