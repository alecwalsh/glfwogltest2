#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class GameObject {
  protected:
    glm::mat4 GetTransform() const;

    double& elapsedTime;
    double& deltaTime;
  public:
    std::string name = "unnamed";

    glm::vec3 position = {0,0,0};
    glm::vec3 scale = {1,1,1};
    glm::mat4 rotation{1.0f}; // TODO: Use quaternion for rotation
    
    GameObject();
    GameObject(glm::vec3 position, glm::vec3 scale);
    virtual ~GameObject();
    GameObject(const GameObject&);

    // Runs every frame
    virtual void Tick() = 0;
};