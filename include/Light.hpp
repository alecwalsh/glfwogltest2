#pragma once

#include <glm/glm.hpp>

class Light {
    static std::int32_t GetLightUniLoc(const char* member, std::size_t i, std::uint32_t program);
  protected:
    static auto GetLightUniLocGenerator(std::uint32_t program, std::size_t index) noexcept {
        return [program, index](const char* member) { return Light::GetLightUniLoc(member, index, program); };
    }
  public:
    enum class LightType : std::uint8_t {
        Point,
        Directional,
        Spot,
    };
    const LightType type;

    void ToggleActive() noexcept { active = !active; }

    virtual ~Light() = default;

    // Set the shader uniforms that are relevant to all light types
    // Overriding subclasses should call this, and then set their own uniforms
    virtual void SetUniforms(std::uint32_t program, std::size_t index);
  protected:
    glm::vec3 diffuse{0.0f};
    glm::vec3 specular{0.0f};

    bool active = true;
    
    Light(glm::vec3 diffuse, glm::vec3 specular, LightType type, bool active = true) noexcept;
};