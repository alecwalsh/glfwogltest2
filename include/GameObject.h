#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// TODO: Make Camera a subclass of GameObject
class GameObject {
  protected:
    glm::mat4 transform{1.0f};

    double& elapsedTime;
    double& deltaTime;
  public:
    std::string name = "";
    
    GameObject(glm::mat4 transform);
    virtual ~GameObject();
    GameObject(const GameObject&);

    // Runs every frame
    virtual void Tick() = 0;

    void SetTransform(const glm::mat4& transform);
    void ModTransform(const glm::mat4& transform);
};