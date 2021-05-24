#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class GameObject {
  protected:
    glm::vec3 position = {0, 0, 0};
    glm::vec3 scale = {1, 1, 1};

    glm::mat4 GetTransform() const;

    double& elapsedTime;
    double& deltaTime;
  public:
    std::string name = "unnamed";

    virtual void SetScale(glm::vec3 scale) { this->scale = scale; }

    virtual glm::vec3 GetScale() const { return scale; }

    virtual void SetPosition(glm::vec3 position) {
        this->position = position;
    }

    virtual glm::vec3 GetPosition() const { return position; }

    virtual void ModifyPosition(glm::vec3 translation) { SetPosition(GetPosition() + translation); }

    glm::mat4 rotation{1.0f}; // TODO: Use quaternion for rotation
    
    GameObject();
    GameObject(glm::vec3 position, glm::vec3 scale);
    virtual ~GameObject();
    GameObject(const GameObject&);

    // Runs every frame
    virtual void Tick() = 0;
};