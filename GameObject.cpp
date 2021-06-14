#include "GameObject.hpp"

#include <iostream>
#include <limits>
#include <cstdlib>

#include "TimeManager.hpp"

glm::mat4 GameObject::GetTransform() const noexcept {
    glm::mat4 scaleMat = glm::scale(glm::mat4{1.0f}, scale);
    glm::mat4 positionMat = glm::translate(glm::mat4{1.0f}, position);

    return positionMat * rotation * scaleMat;
}

GameObject::GameObject()
    : elapsedTime{timeManager.elapsedTime}, deltaTime{timeManager.deltaTime} {
    std::cout << "GameObject constructor" << std::endl;
}

GameObject::GameObject(glm::vec3 position, glm::vec3 scale) : GameObject{} {
    this->position = position;
    this->scale = scale;
}

GameObject::~GameObject() { std::cout << "GameObject destructor" << std::endl; }

// Copy constructor
GameObject::GameObject(const GameObject& rhs)
    : position{rhs.position}, scale{rhs.scale}, elapsedTime{rhs.elapsedTime}, deltaTime{rhs.deltaTime} {
    std::cout << "GameObject copy constructor" << std::endl;
}
