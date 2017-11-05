#include "Light.h"

Light::Light(glm::vec3 diffuse_, glm::vec3 specular_, LightType type_) : type(type_) {
    diffuse = diffuse_;
    specular = specular_;
}

Light::~Light() {}
