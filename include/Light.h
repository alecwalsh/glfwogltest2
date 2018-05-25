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

    glm::vec3 diffuse{0.0f};
    glm::vec3 specular{0.0f};

    bool active = true;

    void ToggleActive() { active = !active; }

    virtual ~Light() = 0;

  protected:
    Light(glm::vec3 diffuse_, glm::vec3 specular_, LightType type_);
};
