#include "PointLight.h"

PointLight::PointLight(glm::vec3 position, glm::vec3 diffuse, glm::vec3 specular)
    : Light{diffuse, specular, LightType::Point}, position(position) {}

PointLight::~PointLight() {}
