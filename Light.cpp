#include "Light.h"

Light::Light(glm::vec3 diffuse, glm::vec3 specular, LightType type) : type(type), diffuse(diffuse), specular(specular) {}