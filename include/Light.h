#pragma once
#include <glm/glm.hpp>

class Light {
public:
    enum class LightType {
        Point,
        Directional,
        Spot,
    };
    const LightType type;
    
    glm::vec3 diffuse;
    glm::vec3 specular;
    
    virtual ~Light();
protected:
    Light(glm::vec3 diffuse_, glm::vec3 specular_, LightType type_);
};
