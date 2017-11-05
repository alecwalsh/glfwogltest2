#pragma once

#include "Light.h"

class DirLight : public Light {
public:
    glm::vec3 direction;

    DirLight(glm::vec3 direction_, glm::vec3 diffuse_, glm::vec3 specular_);
    virtual ~DirLight() override;
};
