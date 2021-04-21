#pragma once

#include <glm/glm.hpp>

#include "glad/glad.h"

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

    virtual ~Light() = default;

    // Set the shader uniforms that are relevant to all light types
    // Overriding subclasses should call this, and then set their own uniforms
    virtual void SetUniforms(GLuint program, std::size_t index);

    static GLint getLightUniLoc(const char* member, std::size_t i, GLuint program);

    static auto getLightUniLocGenerator(GLuint program, std::size_t index) {
        return [=](const char* member) { return Light::getLightUniLoc(member, index, program); };
    }
  protected:
    Light(glm::vec3 diffuse, glm::vec3 specular, LightType type);
};