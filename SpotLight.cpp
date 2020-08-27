#include "SpotLight.h"

SpotLight::SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular,
                     float cutoffAngle)
    : Light{diffuse, specular, LightType::Spot}, position(position), direction(direction),
      cutoffAngle(cutoffAngle) {}
